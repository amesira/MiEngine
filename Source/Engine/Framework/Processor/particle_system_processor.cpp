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
#include "Engine/Framework/Component/particle_system_component.h"

void ParticleSystemProcessor::Initialize()
{
}

void ParticleSystemProcessor::Finalize()
{
}

void ParticleSystemProcessor::Process(IScene* pScene)
{
    auto* particlePool = pScene->GetComponentPool<ParticleSystemComponent>();
    if (!particlePool) return;

    const float deltaTime = FPS_GetDeltaTime();

    auto& particleSystems = particlePool->GetList();
    for (ParticleSystemComponent& particleSystem : particleSystems) {
        // 無効なコンポーネントはスキップ
        if (!particleSystem.GetOwner()->GetActive()) continue;
        if (!particleSystem.GetEnable()) continue;

        // 再生開始
        auto& main = particleSystem.Main();
        if (main.playOnAwake && !particleSystem.IsPlaying()) {
            particleSystem.Play();
        }

        // 再生中でなければスキップ
        if (!particleSystem.IsPlaying()) continue;


    }
}
