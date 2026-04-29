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

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

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
    if (InspectorViewWindow::BeginComponentSection(this, "Player Move")) {
        ImGui::Text("CurrentAngleY: %.2f", XMConvertToDegrees(m_currentAngleY));
    }

    InspectorViewWindow::EndComponentSection();
}

//-----------------------------------------------

// 動き更新処理
void PlayerMoveBehavior::UpdateMove(const PlayerContext& context, float deltaTime, float speedMultipler)
{
    const PlayerInput& input = context.input;

    XMFLOAT3 velocity = m_rigidbody->GetVelocity();

    // 移動更新処理
    {
        XMFLOAT3 moveDirection = { input.moveInputCameraLocal.x, 0.0f, input.moveInputCameraLocal.z };
        moveDirection = MiMath::Multiply(MiMath::Normalize(moveDirection), m_moveSpeed);

        velocity.x += moveDirection.x * deltaTime * 10.0f * speedMultipler;
        velocity.z += moveDirection.z * deltaTime * 10.0f * speedMultipler;

        velocity.x = MiMath::Clamp(velocity.x, -m_moveSpeed * speedMultipler, m_moveSpeed * speedMultipler);
        velocity.z = MiMath::Clamp(velocity.z, -m_moveSpeed * speedMultipler, m_moveSpeed * speedMultipler);
    }

    // ジャンプ処理
    if (input.triggerJumpCommand && m_rigidbody->GetIsGrounded()) {
        velocity.y = m_jumpForce;
    }

    m_rigidbody->SetVelocity(velocity);
}

// 回転更新処理
// ・基本の回転を行う。装飾的な回転はPlayerVisualMachineBehaviorで行う
void PlayerMoveBehavior::UpdateRotation(const PlayerContext& context, float deltaTime)
{
    const XMFLOAT3& move = context.input.moveInputCameraLocal;

    // 入力がほぼない時は向きを変えない
    if (MiMath::Length(XMFLOAT3(move.x, 0.0f, move.z)) < 0.01f) {
        return;
    }

    // XZ平面の向きをそのまま角度にする
    float targetAngleY = atan2f(-move.x, -move.z);

    // 補完
    targetAngleY = MiMath::Lerp(m_currentAngleY, targetAngleY, deltaTime * 10.0f);

    m_transform->SetEulerAngle({ 0.0f, targetAngleY, 0.0f });
    m_currentAngleY = targetAngleY;
}
