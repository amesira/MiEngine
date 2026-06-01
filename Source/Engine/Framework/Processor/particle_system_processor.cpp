//===================================================
// particle_system_processor.cpp
//===================================================
#include "particle_system_processor.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Framework/Component/particle_system_component.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace {
    using ParticleSystem = ParticleSystemComponent;

    std::mt19937& GetRandomEngine()
    {
        static std::mt19937 engine{ std::random_device{}() };
        return engine;
    }

    float RandomRange(float minValue, float maxValue)
    {
        std::uniform_real_distribution<float> dist(minValue, maxValue);
        return dist(GetRandomEngine());
    }

    DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    }

    DirectX::XMFLOAT3 Multiply(const DirectX::XMFLOAT3& value, float scale)
    {
        return { value.x * scale, value.y * scale, value.z * scale };
    }

    float Length(const DirectX::XMFLOAT3& value)
    {
        return std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
    }

    DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& value)
    {
        const float length = Length(value);
        if (length <= 0.0001f) return { 0.0f, 1.0f, 0.0f };
        return { value.x / length, value.y / length, value.z / length };
    }

    DirectX::XMFLOAT3 RandomUnitVector()
    {
        const float z = RandomRange(-1.0f, 1.0f);
        const float angle = RandomRange(0.0f, DirectX::XM_2PI);
        const float radius = std::sqrt((1.0f - z * z) > 0.0f ? (1.0f - z * z) : 0.0f);
        return {
            radius * std::cos(angle),
            radius * std::sin(angle),
            z
        };
    }

    float Evaluate(const ParticleSystem::MinMaxFloat& value)
    {
        if (!value.randomBetweenTwoConstants) return value.constant;
        return RandomRange(value.constantMin, value.constantMax);
    }

    DirectX::XMFLOAT4 Evaluate(const ParticleSystem::MinMaxColor& value)
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

    float EvaluateCurve(const ParticleSystem::FloatCurve& curve, float normalizedTime)
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

    void CreateSpawnTransform(
        const ParticleSystem::ShapeModule& shape,
        DirectX::XMFLOAT3& outPosition,
        DirectX::XMFLOAT3& outDirection)
    {
        outPosition = { 0.0f, 0.0f, 0.0f };
        outDirection = { 0.0f, 1.0f, 0.0f };

        if (!shape.enabled) return;

        if (shape.type == ParticleSystem::ShapeType::Sphere) {
            DirectX::XMFLOAT3 direction = RandomUnitVector();
            const float radius = shape.sphere.emitFromShell
                ? shape.sphere.radius
                : shape.sphere.radius * std::cbrt(RandomRange(0.0f, 1.0f));

            outPosition = Multiply(direction, radius);
            outDirection = direction;
        }
        else if (shape.type == ParticleSystem::ShapeType::Cone) {
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
            outDirection = Normalize({
                spread * std::cos(directionAngle),
                1.0f,
                spread * std::sin(directionAngle)
            });
        }

        if (shape.randomDirectionAmount > 0.0f) {
            const float amount = std::clamp(shape.randomDirectionAmount, 0.0f, 1.0f);
            const DirectX::XMFLOAT3 randomDirection = RandomUnitVector();
            outDirection = Normalize({
                outDirection.x * (1.0f - amount) + randomDirection.x * amount,
                outDirection.y * (1.0f - amount) + randomDirection.y * amount,
                outDirection.z * (1.0f - amount) + randomDirection.z * amount,
            });
        }
    }

    void EmitParticles(ParticleSystem& particleSystem, int count)
    {
        if (count <= 0) return;

        auto& particles = particleSystem.Particles();
        auto& main = particleSystem.Main();
        const auto& shape = particleSystem.Shape();

        for (int i = 0; i < count; ++i) {
            if (static_cast<int>(particles.size()) >= ParticleSystem::MAX_PARTICLES) break;

            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT3 direction;
            CreateSpawnTransform(shape, position, direction);

            ParticleSystem::ParticleData particle;
            particle.alive = true;
            particle.elapsedTime = 0.0f;
            const float startLifetime = Evaluate(main.startLifetime);
            particle.lifetime = startLifetime > 0.0001f ? startLifetime : 0.0001f;
            particle.startSize = Evaluate(main.startSize);
            particle.size = particle.startSize;
            particle.position = position;
            particle.velocity = Multiply(direction, Evaluate(main.startSpeed));
            particle.color = Evaluate(main.startColor);

            particles.push_back(particle);
        }
    }

    bool CrossedTime(float previousTime, float currentTime, float targetTime)
    {
        return previousTime <= targetTime && targetTime < currentTime;
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

        // シミュレーションの更新
        const float scaledDeltaTime = deltaTime * main.simulationSpeed;
        const float previousTime = particleSystem.GetTime();
        float currentTime = previousTime + scaledDeltaTime;

        if (main.duration > 0.0f && currentTime >= main.duration) {
            if (main.loop) {
                currentTime = std::fmod(currentTime, main.duration);
            }
            else {
                currentTime = main.duration;
                particleSystem.Stop();
            }
        }

        particles.reserve(static_cast<size_t>(ParticleSystem::MAX_PARTICLES));

        if (emission.enabled) {
            float accumulator = particleSystem.GetEmitAccumulator();
            accumulator += emission.rateOverTime * scaledDeltaTime;

            const int emitCount = static_cast<int>(accumulator);
            accumulator -= static_cast<float>(emitCount);
            particleSystem.SetEmitAccumulator(accumulator);
            EmitParticles(particleSystem, emitCount);

        }

        for (auto& particle : particles) {
            if (!particle.alive) continue;

            particle.elapsedTime += scaledDeltaTime;
            if (particle.elapsedTime >= particle.lifetime) {
                particle.alive = false;
                continue;
            }

            particle.velocity = Add(particle.velocity, Multiply(main.gravity, scaledDeltaTime));
            particle.position = Add(particle.position, Multiply(particle.velocity, scaledDeltaTime));

            if (particleSystem.SizeOverLifetime().enabled) {
                const float normalizedAge = particle.elapsedTime / particle.lifetime;
                particle.size = particle.startSize * EvaluateCurve(particleSystem.SizeOverLifetime().size, normalizedAge);
            }
        }

        particles.erase(
            std::remove_if(particles.begin(), particles.end(), [](const ParticleSystem::ParticleData& particle) {
                return !particle.alive;
            }),
            particles.end());

        particleSystem.SetTime(currentTime);
    }
}
