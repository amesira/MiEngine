//===================================================
// player_move_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//===================================================
#include "player_move_behavior.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Utility/mi_math.h"
#include "Engine/System/Device/mi_fps.h"

#include "Engine/Editor/imgui_window_interface.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"

void PlayerMoveBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;

    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
}

void PlayerMoveBehavior::Update()
{

}

void PlayerMoveBehavior::DrawComponentInspector()
{

}

//-----------------------------------------------

// 動き更新処理
void PlayerMoveBehavior::UpdateMove(const PlayerContext& context, float deltaTime)
{
    const PlayerInput& input = context.input;

    XMFLOAT3 velocity = m_rigidbody->GetVelocity();

    // 移動更新処理
    {
        XMFLOAT3 moveDirection = { input.moveInput.x, 0.0f, input.moveInput.z };
        moveDirection = MiMath::Multiply(MiMath::Normalize(moveDirection), m_moveSpeed);

        velocity.x += moveDirection.x * deltaTime * 10.0f; // 加速度的に速度を増加させるために10倍する
        velocity.z += moveDirection.z * deltaTime * 10.0f;

        velocity.x = MiMath::Clamp(velocity.x, -m_moveSpeed, m_moveSpeed);
        velocity.z = MiMath::Clamp(velocity.z, -m_moveSpeed, m_moveSpeed);
    }

    // ジャンプ処理
    if (input.triggerJumpCommand && m_rigidbody->GetIsGrounded()) {
        velocity.y = m_jumpForce;
    }

    m_rigidbody->SetVelocity(velocity);
}

// 回転更新処理
void PlayerMoveBehavior::UpdateRotation(const PlayerContext& context, float deltaTime)
{
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    
    // 速度方向に応じてプレイヤーの向きを変える
    if (MiMath::Length(XMFLOAT3(velocity.x, 0.0f, velocity.z)) > 0.2f) {
        float angle = atan2f(velocity.x, velocity.z) + XM_PI;
        float newAngleY = MiMath::Lerp(m_currentAngleY, angle, std::clamp(deltaTime * 10.0f, 0.0f, 1.0f));
    
        m_transform->SetEulerAngle({ 0.0f, newAngleY, 0.0f });
    
        m_currentAngleY = newAngleY;
    }
    else {
        // 入力がないときはアニメーションをIdleにするなどの処理をここで行う
    }
}
