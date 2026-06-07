// PrefabFactory.cpp
#include "prefab_factory.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_base.h"

// component
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/decal_component.h"
#include "Engine/Framework/Component/particle_system_component.h"

#include "Game/Behavior/transform_constraint_behavior.h"
#include "Game/Behavior/PlayerBehavior/player_behavior.h"

#include "Engine/engine_service_locator.h"
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

#include "actor_factory.h"
#include "render_effect_factory.h"


namespace PrefabFactory
{
    // TransformConstraintBehaviorのセットアップ
    void SetupTransformConstraint(GameObject* element, 
        TransformComponent* target, const XMFLOAT3& positionOffset, 
        bool considerRotation = true, bool considerScale = false) 
    {
        TransformConstraintBehavior* constraint = element->AddComponent<TransformConstraintBehavior>();
        constraint->SetTarget(target);
        constraint->SetOffset(positionOffset);
        constraint->SetConsiderRotation(considerRotation);
        constraint->SetConsiderScaling(considerScale);
    }

    // ----------------------------------- プレハブ生成

    // プレイヤープレハブ生成
    PlayerPrefab PrefabFactory::CreatePlayerPrefab(SceneBase* scene, const XMFLOAT3& position)
    {
        PlayerPrefab prefab;
        prefab.player = ActorFactory::CreatePlayer(scene, position);
        TransformComponent* playerTransform = prefab.player->GetComponent<TransformComponent>();
        PlayerBehavior* playerBehavior = prefab.player->GetComponent<PlayerBehavior>();

        prefab.runDustParticle = RenderEffectFactory::CreateRunDustParticle(scene, prefab.player->GetName(), L"asset\\Texture\\white.bmp");
        {
            SetupTransformConstraint(prefab.runDustParticle, playerTransform, { 0.0f, -1.0f, 0.0f }, true, false);
        }
        prefab.chargeEffectParticle = RenderEffectFactory::CreateChargeAbsorbParticle(scene, position, L"asset\\Texture\\white.bmp");
        {
            SetupTransformConstraint(prefab.chargeEffectParticle, playerTransform, { 1.0f, -0.3f, 0.0f }, true, false);
            ParticleSystemComponent* particleSystem = prefab.chargeEffectParticle->GetComponent<ParticleSystemComponent>();
            particleSystem->Main().playOnAwake = false; // 最初は再生しない
            playerBehavior->SetupChargeEffect(particleSystem);
        }

        return prefab;
    }
}