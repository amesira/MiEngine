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
#include "Engine/Device/mi_fps.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/camera_component.h"

#include <cmath>

void PlayerMoveBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;

    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();

    IScene* scene = owner->GetScene();

    // メインカメラの参照取得
    {
        GameObject* mainCamera = scene->GetGameObjectByName("MainCamera");
        if (mainCamera) {
            m_mainCamera = mainCamera->GetComponent<CameraComponent>();
        }
    }
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
void PlayerMoveBehavior::UpdateRotation(const PlayerContext&, float deltaTime)
{
    if (!m_transform || !m_mainCamera) return;

    const XMFLOAT3& cameraForward = m_mainCamera->GetForward();

    // ビルボード回転の計算
    float billboardAngleY = atan2f(cameraForward.x, cameraForward.z);
    XMFLOAT4 targetRotation = MiMath::QuaternionFromEuler({ 0.0f, billboardAngleY, 0.0f });

    // Slerpで回転をスムーズに追従
    XMFLOAT4 currentRotation = m_transform->GetRotation();
    currentRotation = MiMath::Slerp(currentRotation, targetRotation, m_rotationSpeed * deltaTime);

    // 回転の適用
    m_transform->SetRotation(currentRotation);
    m_currentAngleY = m_transform->GetEulerAngle().y;
}
