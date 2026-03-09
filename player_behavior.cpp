//===================================================
// player_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "player_behavior.h"
#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"

#include "mi_math.h"
#include "keyboard.h"

#include "transform_component.h"
#include "rigidbody_component.h"

PlayerBehavior::PlayerBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<PlayerBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
}

PlayerBehavior::~PlayerBehavior()
{
}

void PlayerBehavior::Update(IScene* pScene)
{
    if (Keyboard_IsKeyDown(KK_W)) {
        m_moveDirection.z = 1.0f;
    }
    if (Keyboard_IsKeyDown(KK_S)) {
        m_moveDirection.z = -1.0f;
    }
    if (Keyboard_IsKeyDown(KK_A)) {
        m_moveDirection.x = -1.0f;
    }
    if (Keyboard_IsKeyDown(KK_D)) {
        m_moveDirection.x = 1.0f;
    }

    // 速度設定
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    velocity.x = m_moveDirection.x * 2.0f;
    velocity.z = m_moveDirection.z * 2.0f;

    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        velocity.y = 5.0f;
    }

    m_rigidbody->SetVelocity(velocity);
}