//===================================================
// particle_system_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//===================================================
#include "particle_system_processor.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Device/mi_fps.h"

#include "Utility/mi_math.h"
#include "Utility/mi_curve.h"
using namespace MiMath;

#include <algorithm>
#include <cmath>
#include <random>

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/particle_system_component.h"

namespace {
    // ------------------------------------ Evaluate
    // MinMaxFloatからランダムな値を生成
    float Evaluate(const ParticleSystemData::MinMaxFloat& value)
    {
        if (!value.randomBetweenTwoConstants) return value.constant;
        return RandomRange(value.constantMin, value.constantMax);
    }
    // MinMaxColorからランダムな色を生成
    XMFLOAT4 Evaluate(const ParticleSystemData::MinMaxColor& value)
    {
        if (!value.randomBetweenTwoColors) return value.color;

        const float t = RandomRange(0.0f, 1.0f);
        return {
            value.colorMin.x + (value.colorMax.x - value.colorMin.x) * t,
            value.colorMin.y + (value.colorMax.y - value.colorMin.y) * t,
            value.colorMin.z + (value.colorMax.z - value.colorMin.z) * t,
            value.colorMin.w + (value.colorMax.w - value.colorMin.w) * t,
        };
    }

    // ------------------------------------ EmitParticles
    // 発生形状に基づいて発生位置と発生方向を生成
    void CreateSpawnTransform(
        const ParticleSystemData::ShapeModule& shape,
        XMFLOAT3& outPosition,
        XMFLOAT3& outDirection)
    {
        outPosition = { 0.0f, 0.0f, 0.0f };
        outDirection = { 0.0f, 1.0f, 0.0f };

        if (!shape.enabled) return;

        // Sphere形状の発生
        if (shape.type == ParticleSystemData::ShapeType::Sphere) {
            DirectX::XMFLOAT3 direction = RandomUnitVector();
            const float radius = shape.sphere.emitFromShell
                ? shape.sphere.radius
                : shape.sphere.radius * std::cbrt(RandomRange(0.0f, 1.0f));

            outPosition = Multiply(direction, radius);
            outDirection = direction;
        }
        // Cone形状の発生
        else if (shape.type == ParticleSystemData::ShapeType::Cone) {
            const float baseAngle = RandomRange(0.0f, DirectX::XM_2PI);
            const float baseRadius = shape.cone.radius * std::sqrt(RandomRange(0.0f, 1.0f));

            if (shape.cone.emitFromBase) {
                outPosition = {
                    baseRadius * std::cos(baseAngle),
                    0.0f,
                    baseRadius * std::sin(baseAngle)
                };
            }

            const float coneAngle = RandomRange(0.0f, shape.cone.angle);
            const float spread = std::tan(coneAngle);
            const float directionAngle = RandomRange(0.0f, DirectX::XM_2PI);

            outDirection = {
                spread * std::cos(directionAngle),
                1.0f,
                spread * std::sin(directionAngle)
            };
            outDirection = Normalize(outDirection);
        }

        // 発生方向のランダムさを加える
        if (shape.randomDirectionAmount > 0.0f) {
            const float amount = std::clamp(shape.randomDirectionAmount, 0.0f, 1.0f);
            const DirectX::XMFLOAT3 randomDirection = RandomUnitVector();
            outDirection = {
                outDirection.x * (1.0f - amount) + randomDirection.x * amount,
                outDirection.y * (1.0f - amount) + randomDirection.y * amount,
                outDirection.z * (1.0f - amount) + randomDirection.z * amount,
            };
            outDirection = Normalize(outDirection);
        }
    }

    // 指定した数のパーティクルを発生させる
    void EmitParticles(ParticleSystemComponent& particleSystem, XMFLOAT3 emitterPosition, int count)
    {
        if (count <= 0) return;

        auto& particles = particleSystem.Particles();
        auto& desc = particleSystem.GetDesc();
        auto& main = desc.mainModule;
        const auto& shape = desc.shapeModule;

        for (int i = 0; i < count; i++) {
            if (static_cast<int>(particles.size()) >= ParticleSystemComponent::MAX_PARTICLES) break;

            // 発生位置と発生方向を決定
            XMFLOAT3 position;
            XMFLOAT3 direction;
            CreateSpawnTransform(shape, position, direction);

            // パーティクルの初期パラメータを設定
            ParticleSystemComponent::ParticleData particle;
            particle.alive = true;
            particle.elapsedTime = 0.0f;
            const float startLifetime = Evaluate(main.startLifetime);
            particle.lifetime = startLifetime > 0.0001f ? startLifetime : 0.0001f;

            particle.startSize = Evaluate(main.startSize);
            particle.size = particle.startSize;

            particle.position = Add(position, emitterPosition);
            particle.velocity = Multiply(direction, Evaluate(main.startSpeed));
            particle.color = Evaluate(main.startColor);

            // パーティクルを追加
            particles.push_back(particle);
        }
    }
}

void ParticleSystemProcessor::Initialize()
{
}

void ParticleSystemProcessor::Finalize()
{
}

void ParticleSystemProcessor::Process(IScene* pScene)
{
    if (!pScene) return;

    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* particlePool = pScene->GetComponentPool<ParticleSystemComponent>();
    if (!transformPool || !particlePool) return;

    const float deltaTime = FPS_GetDeltaTime();

    auto& particleSystems = particlePool->GetList();
    for (ParticleSystemComponent& particleSystem : particleSystems) {
        TransformComponent* transform = transformPool->GetByGameObjectID(particleSystem.GetOwner()->GetID());
        if (!transform) continue;
        if (!particleSystem.GetOwner()->GetActive()) continue;
        if (!particleSystem.GetEnable()) continue;

        auto& desc = particleSystem.GetDesc();
        auto& main = desc.mainModule;
        auto& emission = desc.emissionModule;
        auto& particles = particleSystem.Particles();

        // playOnAwakeが有効で、まだ再生されていない場合は再生する
        if (main.playOnAwake && !particleSystem.IsPlaying()) {
            particleSystem.Play();

            particleSystem.SetPreviousPosition(transform->GetPosition());
        }

        // 再生中でなければスキップ
        if (!particleSystem.IsPlaying()) continue;

        // 現在位置を更新・前フレームの位置を取得
        XMFLOAT3 currentPosition = transform->GetPosition();
        particleSystem.SetCurrentPosition(currentPosition);

        XMFLOAT3 previousPosition = particleSystem.GetPreviousPosition();

        // === シミュレーションの更新 ===
        const float scaledDeltaTime = deltaTime * main.simulationSpeed;
        const float previousTime = particleSystem.GetTime();
        float currentTime = previousTime + scaledDeltaTime;

        // durationを超えたらループするか停止する
        if (main.duration > 0.0f && currentTime >= main.duration) {
            if (main.loop) {
                currentTime = std::fmod(currentTime, main.duration);
            }
            else {
                currentTime = main.duration;

                emission.enabled = false; // エミッションを停止
            }
        }

        // パーティクルの領域確保
        particles.reserve(static_cast<size_t>(ParticleSystemComponent::MAX_PARTICLES));

        // === パーティクルの更新 ===
        for (auto& particle : particles) {
            if (!particle.alive) continue;

            // 経過時間の更新と寿命のチェック
            particle.elapsedTime += scaledDeltaTime;
            if (particle.elapsedTime >= particle.lifetime) {
                particle.alive = false;
                continue;
            }

            // 速度と位置の更新
            particle.velocity = Add(particle.velocity, Multiply(main.gravity, scaledDeltaTime));
            particle.position = Add(particle.position, Multiply(particle.velocity, scaledDeltaTime));
            if (main.simulationSpace == ParticleSystemData::SimulationSpace::Local) {
                particle.position = Add(particle.position, Subtract(currentPosition, previousPosition));
            }

            // サイズの更新 --- SizeOverLifeTime ---
            auto& sizeOverLifetime = desc.sizeOverLifetimeModule;
            if (sizeOverLifetime.enabled) {
                const float normalizedAge = particle.elapsedTime / particle.lifetime;
                particle.size = particle.startSize * MiCurve::Evaluate(sizeOverLifetime.size, normalizedAge);
            }
        }

        // === エミッション ===
        if (emission.enabled) {
            float accumulator = particleSystem.GetEmitAccumulator();
            // --- 時間ベースの発生
            accumulator += emission.rateOverTime * scaledDeltaTime;

            // 1.0f以上溜まっている分だけパーティクルを発生させる
            int emitCount = static_cast<int>(accumulator);
            accumulator -= static_cast<float>(emitCount);
            particleSystem.SetEmitAccumulator(accumulator);
            EmitParticles(particleSystem, currentPosition, emitCount);

            // --- 距離ベースの発生
            accumulator = particleSystem.GetDistanceAccumulator();
            accumulator += Length(Subtract(currentPosition, previousPosition)) * emission.rateOverDistance; 
            
            emitCount = int(accumulator);
            accumulator -= static_cast<int>(emitCount);
            EmitParticles(particleSystem, currentPosition, emitCount);

            particleSystem.SetDistanceAccumulator(accumulator);
        }

        particles.erase(
            std::remove_if(particles.begin(), particles.end(), [](const ParticleSystemComponent::ParticleData& particle) {
                return !particle.alive;
                }),
            particles.end());

        // === 時間の更新 ===
        particleSystem.SetTime(currentTime);

        // 前フレームの位置を更新
        particleSystem.SetPreviousPosition(currentPosition);
    }
}