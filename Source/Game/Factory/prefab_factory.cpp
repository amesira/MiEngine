// PrefabFactory.cpp
#include "prefab_factory.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_base.h"

// component
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/decal_component.h"
#include "Engine/Framework/Component/particle_system_component.h"
#include "Engine/Framework/Component/light_component.h"
#include "Engine/Framework/Component/rect_transform_component.h"

#include "Game/Behavior/transform_constraint_behavior.h"
#include "Game/Behavior/PlayerBehavior/player_behavior.h"
#include "Game/Behavior/PlayerBehavior/PlayerState/player_attack_behavior.h"
#include "Game/Behavior/BulletBehavior/bezier_line_preview_behavior.h"
#include "Game/Behavior/BaseBehavior/health_behavior.h"

#include "Engine/engine_service_locator.h"
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

#include "actor_factory.h"
#include "render_effect_factory.h"
#include "projectile_factory.h"
#include "environment_factory.h"
#include "ui_factory.h"

#include <array>

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
        PlayerAttackBehavior* playerAttackBehavior = prefab.player->GetComponent<PlayerAttackBehavior>();

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
        prefab.chargeLight = EnvironmentFactory::CreatePointLight(scene, { 1.0f, 0.5f, 0.0f, 1.0f }, 3.0f);
        {
            SetupTransformConstraint(prefab.chargeLight, playerTransform, { 1.0f, -0.3f, -0.5f }, true, false);
            LightComponent* lightComp = prefab.chargeLight->GetComponent<LightComponent>();
            lightComp->SetEnable(false); // 最初はライトをオフにする
            playerBehavior->SetupChargeLight(lightComp);
        }
        if (playerAttackBehavior) {
            std::array<BezierLinePreviewBehavior*, PlayerAttackBehavior::MISSILE_PREVIEW_LINE_COUNT> previewLines = {};

            for (int i = 0; i < PlayerAttackBehavior::MISSILE_PREVIEW_LINE_COUNT; ++i) {
                ProjectileFactory::BezierLinePreviewCreateDesc lineDesc;
                lineDesc.name = "MissilePreviewLine";
                lineDesc.visibleOnCreate = false;
                lineDesc.lineWidth = 5.0f;

                GameObject* lineObject = ProjectileFactory::CreateBezierLinePreview(scene, lineDesc);
                if (!lineObject) continue;

                previewLines[i] = lineObject->GetComponent<BezierLinePreviewBehavior>();
            }

            playerAttackBehavior->SetupMissilePreviewLines(previewLines);
        }

        return prefab;
    }

    EnemyPrefab PrefabFactory::CreateEnemyPrefab(SceneBase* scene, const XMFLOAT3& position)
    {
        EnemyPrefab prefab;
        prefab.enemy = ActorFactory::CreateSimpleEnemy(scene, position);
        prefab.healthBar = nullptr;

        return prefab;
    }
}
