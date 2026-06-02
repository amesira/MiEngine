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

#include "Engine/engine_service_locator.h"

// プレイヤー生成
GameObject* ActorFactory::CreatePlayer(SceneBase* scene, const XMFLOAT3& position)
{
    GameObject* player = scene->CreateGameObject();
    player->SetName("Player");

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

    return player;
}
