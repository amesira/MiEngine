//===================================================
// actor_factory.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//===================================================
#include "actor_factory.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_base.h"

// component
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/collider_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/animation_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"
#include "Engine/Framework/Component/sprite_animation_component.h"

// behavior
#include "Game/Behavior/PlayerBehavior/player_behavior.h"
#include "Game/Behavior/PlayerBehavior/player_state_machine_behavior.h"
#include "Game/Behavior/PlayerBehavior/player_combat_machine_behavior.h"

#include "Game/Behavior/PlayerBehavior/PlayerState/player_move_behavior.h"
#include "Game/Behavior/PlayerBehavior/PlayerState/player_attack_behavior.h"
#include "Game/Behavior/PlayerBehavior/PlayerState/player_dodge_behavior.h"

#include "Game/Behavior/BaseBehavior/afterimage_generator_behavior.h"

#include "Game/Behavior/BaseBehavior/hit_stop_behavior.h"
#include "Game/Behavior/EnemyBehavior/enemy_behavior.h"
#include "Game/Behavior/EnemyBehavior/enemy_state_machine_behavior.h"
#include "Game/Behavior/EnemyBehavior/base_enemy_attack_behavior.h"

#include "Game/Behavior/BaseBehavior/health_behavior.h"
#include "Game/Behavior/BaseBehavior/blinker_behavior.h"
#include "Game/Behavior/BaseBehavior/shake_object_behavior.h"

#include "Engine/engine_service_locator.h"

// プレイヤー生成
GameObject* ActorFactory::CreatePlayer(SceneBase* scene, const XMFLOAT3& position)
{
    GameObject* player = scene->CreateGameObject();
    player->SetName("Player");
    player->SetCollisionLayer(CollisionLayer::Player);
    player->SetRenderLayer(RenderLayer::Player);

    // component生成・登録
    TransformComponent* transform = player->AddComponent<TransformComponent>();
    BoxColliderComponent* collider = player->AddComponent<BoxColliderComponent>();
    RigidbodyComponent* rigidbody = player->AddComponent<RigidbodyComponent>();
    SpriteRendererComponent* spriteRenderer = player->AddComponent<SpriteRendererComponent>();
    SpriteAnimationComponent* spriteAnimation = player->AddComponent<SpriteAnimationComponent>();

    const XMFLOAT3& playerScaling = { 2.0f, 2.0f, 2.0f };

    // component設定
    transform->SetPosition(position);
    transform->SetScaling(playerScaling);

    rigidbody->SetMass(3.0f);
    rigidbody->SetFriction({ 0.8f, 1.0f, 0.8f });

    collider->SetScale({
        playerScaling.x,
        playerScaling.y,
        playerScaling.z
        });
    collider->SetCenter({ 0.0f, 0.0f, 0.0f });

    MaterialResource mat = {};
    mat.name = "PlayerMaterial";
    spriteRenderer->SetMaterialResource(EngineServiceLocator::GetMaterialRepository()->GenerateMaterial(mat));

    TextureResource* texture = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Texture\\player_sheet.png");
    spriteRenderer->SetTextureResource(texture);
    spriteRenderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    spriteRenderer->SetUvRect({ 0.0f, 0.0f, 0.5f, 0.5f });

    SpriteAnimationComponent::Clip idleClip;
    {
        idleClip.name = "Idle";
        idleClip.frames = {
            { texture, {0.0f, 0.0f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.2f },
            { texture, {0.0f, 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.2f },
        };
        idleClip.speed = 1.0f;
        idleClip.loop = true;
    }
    spriteAnimation->AddClip(idleClip);

    SpriteAnimationComponent::Clip runClip;
    {
        runClip.name = "Run";
        runClip.frames = {
            { texture, {0.0f, 0.0f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.1f },
            { texture, {0.5f, 0.0f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.1f },
            { texture, {0.0f, 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.1f },
        };
        runClip.speed = 1.0f;
        runClip.loop = true;
    }
    spriteAnimation->AddClip(runClip);

    // behavior生成・登録
    player->AddComponent<PlayerBehavior>();

    player->AddComponent<PlayerStateMachineBehavior>();
    player->AddComponent<PlayerCombatMachineBehavior>();

    player->AddComponent<PlayerMoveBehavior>();
    player->AddComponent<PlayerAttackBehavior>();
    player->AddComponent<PlayerDodgeBehavior>();

    player->AddComponent<HitStopBehavior>();
    AfterimageGeneratorBehavior* afterimage = player->AddComponent<AfterimageGeneratorBehavior>();
    {
        afterimage->SetSource(transform, spriteRenderer);
        afterimage->SetEmitInterval(0.1f);
        afterimage->SetAfterimageLifeTime(0.25f);
        afterimage->SetAlphaRange(0.15f, 0.0f);
        afterimage->StopEmission();
    }

    return player;
}

// シンプルな敵生成
GameObject* ActorFactory::CreateSimpleEnemy(SceneBase* scene, const XMFLOAT3& position)
{
    GameObject* enemy = scene->CreateGameObject();
    enemy->SetName("Enemy");
    enemy->SetCollisionLayer(CollisionLayer::Enemy);
    enemy->SetRenderLayer(RenderLayer::Enemy);

    TransformComponent* transform = enemy->AddComponent<TransformComponent>();
    BoxColliderComponent* collider = enemy->AddComponent<BoxColliderComponent>();
    RigidbodyComponent* rigidbody = enemy->AddComponent<RigidbodyComponent>();
    SpriteRendererComponent* spriteRenderer = enemy->AddComponent<SpriteRendererComponent>();
    SpriteAnimationComponent* spriteAnimation = enemy->AddComponent<SpriteAnimationComponent>();

    const XMFLOAT3& enemyScaling = { 2.0f, 2.0f, 2.0f };

    transform->SetPosition(position);
    transform->SetScaling(enemyScaling);

    rigidbody->SetMass(3.0f);
    rigidbody->SetFriction({ 0.8f, 1.0f, 0.8f });

    collider->SetScale({
        enemyScaling.x,
        enemyScaling.y * 2.0f,
        enemyScaling.z
        });
    collider->SetCenter({ 0.0f, 0.0f, 0.0f });

    MaterialResource mat = {};
    mat.name = "EnemyMaterial";
    spriteRenderer->SetMaterialResource(EngineServiceLocator::GetMaterialRepository()->GenerateMaterial(mat));

    TextureResource* texture = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Texture\\enemy_sheet.png");
    spriteRenderer->SetTextureResource(texture);
    spriteRenderer->SetColor({ 1.0f, 0.85f, 0.85f, 1.0f });
    spriteRenderer->SetUvRect({ 0.0f, 0.0f, 0.5f, 1.0f });

    SpriteAnimationComponent::Clip idleClip;
    {
        idleClip.name = "Idle";
        idleClip.frames = {
            { texture, {0.0f, 0.0f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.2f },
            { texture, {0.5f, 0.0f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.2f },
        };
        idleClip.speed = 1.0f;
        idleClip.loop = true;
    }
    spriteAnimation->AddClip(idleClip);

    SpriteAnimationComponent::Clip runClip;
    {
        runClip.name = "Run";
        runClip.frames = {
            { texture, {0.0f, 0.0f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.2f },
            { texture, {0.5f, 0.0f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.2f },
        };
        runClip.speed = 1.0f;
        runClip.loop = true;
    }
    spriteAnimation->AddClip(runClip);
    spriteAnimation->Play("Idle");

    enemy->AddComponent<EnemyBehavior>();
    enemy->AddComponent<EnemyStateMachineBehavior>();
    enemy->AddComponent<BaseEnemyAttackBehavior>();

    enemy->AddComponent<HitStopBehavior>();
    enemy->AddComponent<HealthBehavior>();
    enemy->AddComponent<BlinkerBehavior>();
    enemy->AddComponent<ShakeObjectBehavior>();

    return enemy;
}
