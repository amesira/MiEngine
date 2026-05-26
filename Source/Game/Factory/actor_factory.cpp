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

// behavior
#include "Game/Behavior/PlayerBehavior/player_behavior.h"
#include "Game/Behavior/PlayerBehavior/player_state_machine_behavior.h"
#include "Game/Behavior/PlayerBehavior/player_combat_machine_behavior.h"
#include "Game/Behavior/PlayerBehavior/player_visual_machine_behavior.h"

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
    //ModelComponent* modelComp = player->AddComponent<ModelComponent>();
    RigidbodyComponent* rigidbody = player->AddComponent<RigidbodyComponent>();
    //AnimationComponent* animation = player->AddComponent<AnimationComponent>();

    SpriteRendererComponent* spriteRenderer = player->AddComponent<SpriteRendererComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling({ 1.0f, 1.0f, 1.0f });

    rigidbody->SetMass(3.0f);
    rigidbody->SetFriction({ 0.8f, 1.0f, 0.8f });

    XMFLOAT3 scaling = { 0.8f, 0.8f, 0.8f };
    transform->SetScaling(scaling);
    collider->SetScale({
        scaling.x * 1.0f / 0.8f,
        scaling.y * 2.5f / 0.8f,
        scaling.z * 1.0f / 0.8f
        });
    collider->SetCenter({ 0.0f, collider->GetScale().y / 5.0f * 3.0f, 0.0f });

    spriteRenderer->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    TextureResource* texture = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Texture\\tile_0040.png");
    spriteRenderer->SetTextureResource(texture);

    // behavior生成・登録
    player->AddComponent<PlayerBehavior>();

    player->AddComponent<PlayerStateMachineBehavior>();
    player->AddComponent<PlayerCombatMachineBehavior>();
    player->AddComponent<PlayerVisualMachineBehavior>();

    player->AddComponent<PlayerMoveBehavior>();
    player->AddComponent<PlayerAttackBehavior>();
    player->AddComponent<PlayerDodgeBehavior>();

    return player;
}
