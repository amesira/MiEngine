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
using namespace MiMath;

#include <algorithm>
#include <cmath>
#include <random>

#include "Engine/Framework/Component/particle_system_component.h"

namespace {
    // ------------------------------------ Evaluate
    // MinMaxFloatからランダムな値を生成
    float Evaluate(const ParticleSystemComponent::MinMaxFloat& value)
    {
        if (!value.randomBetweenTwoConstants) return value.constant;
        return RandomRange(value.constantMin, value.constantMax);
    }
    // MinMaxColorからランダムな色を生成
    XMFLOAT4 Evaluate(const ParticleSystemComponent::MinMaxColor& value)
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
    // FloatCurveを評価して値を返す
    float EvaluateCurve(const ParticleSystemComponent::FloatCurve& curve, float normalizedTime)
    {
        if (curve.keys.empty()) return 1.0f;
        if (curve.keys.size() == 1) return curve.keys.front().value;

        normalizedTime = std::clamp(normalizedTime, 0.0f, 1.0f);
        const auto& keys = curve.keys;

        if (normalizedTime <= keys.front().time) return keys.front().value;
        if (normalizedTime >= keys.back().time) return keys.back().value;

        for (size_t i = 1; i < keys.size(); ++i) {
            if (normalizedTime > keys[i].time) continue;

            const auto& prev = keys[i - 1];
            const auto& next = keys[i];
            const float range = (next.time - prev.time) > 0.0001f ? (next.time - prev.time) : 0.0001f;
            const float t = (normalizedTime - prev.time) / range;
            return prev.value + (next.value - prev.value) * t;
        }

        return keys.back().value;
    }

    // ------------------------------------ EmitParticles
    // 発生形状に基づいて発生位置と発生方向を生成
    void CreateSpawnTransform(
        const ParticleSystemComponent::ShapeModule& shape,
        XMFLOAT3& outPosition,
        XMFLOAT3& outDirection)
    {
        outPosition = { 0.0f, 0.0f, 0.0f };
        outDirection = { 0.0f, 1.0f, 0.0f };

        if (!shape.enabled) return;

        // Sphere形状の発生
        if (shape.type == ParticleSystemComponent::ShapeType::Sphere) {
            DirectX::XMFLOAT3 direction = RandomUnitVector();
            const float radius = shape.sphere.emitFromShell
                ? shape.sphere.radius
                : shape.sphere.radius * std::cbrt(RandomRange(0.0f, 1.0f));

            outPosition = Multiply(direction, radius);
            outDirection = direction;
        }
        // Cone形状の発生
        else if (shape.type == ParticleSystemComponent::ShapeType::Cone) {
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
    void EmitParticles(ParticleSystemComponent& particleSystem, int count)
    {
        if (count <= 0) return;

        auto& particles = particleSystem.Particles();
        auto& main = particleSystem.Main();
        const auto& shape = particleSystem.Shape();

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

            particle.position = position;
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

    auto* particlePool = pScene->GetComponentPool<ParticleSystemComponent>();
    if (!particlePool) return;

    const float deltaTime = FPS_GetDeltaTime();

    auto& particleSystems = particlePool->GetList();
    for (ParticleSystemComponent& particleSystem : particleSystems) {
        if (!particleSystem.GetOwner()->GetActive()) continue;
        if (!particleSystem.GetEnable()) continue;

        auto& main = particleSystem.Main();
        auto& emission = particleSystem.Emission();
        auto& particles = particleSystem.Particles();

        // playOnAwakeが有効で、まだ再生されていない場合は再生する
        if (main.playOnAwake && !particleSystem.IsPlaying()) {
            particleSystem.Play();
        }

        // 再生中でなければスキップ
        if (!particleSystem.IsPlaying()) continue;

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
                particleSystem.Stop();
            }
        }

        // パーティクルの領域確保
        particles.reserve(static_cast<size_t>(ParticleSystemComponent::MAX_PARTICLES));

        // === エミッション ===
        if (emission.enabled) {
            float accumulator = particleSystem.GetEmitAccumulator();
            accumulator += emission.rateOverTime * scaledDeltaTime;

            // 1.0f以上溜まっている分だけパーティクルを発生させる
            const int emitCount = static_cast<int>(accumulator);
            accumulator -= static_cast<float>(emitCount);
            particleSystem.SetEmitAccumulator(accumulator);
            EmitParticles(particleSystem, emitCount);
        }

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

            // サイズの更新 --- SizeOverLifeTime ---
            if (particleSystem.SizeOverLifetime().enabled) {
                const float normalizedAge = particle.elapsedTime / particle.lifetime;
                particle.size = particle.startSize * EvaluateCurve(particleSystem.SizeOverLifetime().size, normalizedAge);
            }
        }

        particles.erase(
            std::remove_if(particles.begin(), particles.end(), [](const ParticleSystemComponent::ParticleData& particle) {
                return !particle.alive;
            }),
            particles.end());

        // === 時間の更新 ===
        particleSystem.SetTime(currentTime);
    }
}
