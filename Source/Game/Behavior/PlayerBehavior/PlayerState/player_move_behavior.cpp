//===================================================
// player_move_behavior.cpp
// 
// Author・Miu Kitamura
// Date  ・・026/03/25
//===================================================
#include "player_move_behavior.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Utility/mi_math.h"

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

    if (scene) {
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

void PlayerMoveBehavior::UpdateMove(const PlayerContext& context, const PlayerMoveRequest& moveRequest, float deltaTime)
{
    if (!m_rigidbody) return;

    const PlayerInput& input = context.input;
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();

    if (!moveRequest.canMove) {
        velocity.x = 0.0f;
        velocity.z = 0.0f;
        m_rigidbody->SetVelocity(velocity);
        return;
    }

    XMFLOAT3 moveDirection = { input.moveInputCameraLocal.x, 0.0f, input.moveInputCameraLocal.z };
    moveDirection = MiMath::Multiply(MiMath::Normalize(moveDirection), m_moveSpeed);

    velocity.x += moveDirection.x * deltaTime * 10.0f * moveRequest.speedMultiplier;
    velocity.z += moveDirection.z * deltaTime * 10.0f * moveRequest.speedMultiplier;

    velocity.x = MiMath::Clamp(velocity.x, -m_moveSpeed * moveRequest.speedMultiplier, m_moveSpeed * moveRequest.speedMultiplier);
    velocity.z = MiMath::Clamp(velocity.z, -m_moveSpeed * moveRequest.speedMultiplier, m_moveSpeed * moveRequest.speedMultiplier);

    if (input.triggerJumpCommand && m_rigidbody->GetIsGrounded()) {
        velocity.y = m_jumpForce;
    }

    m_rigidbody->SetVelocity(velocity);
}

void PlayerMoveBehavior::UpdateRotation(const PlayerContext& context, const PlayerMoveRequest& moveRequest, float deltaTime)
{
    if (!moveRequest.canRotate) return;
    if (moveRequest.rotationMode == PlayerRotationMode::Locked) return;
    if (!m_transform || !m_mainCamera) return;

    XMFLOAT3 forward = m_mainCamera->GetForward();

    switch (moveRequest.rotationMode) {
    case PlayerRotationMode::MoveDirection:
        if (MiMath::Length(context.input.moveInputCameraLocal) > 0.01f) {
            forward = context.input.moveInputCameraLocal;
        }
        break;

    case PlayerRotationMode::CameraForward:
    case PlayerRotationMode::AimForward:
    default:
        break;
    }

    const float billboardAngleY = atan2f(forward.x, forward.z);
    XMFLOAT4 targetRotation = MiMath::QuaternionFromEuler({ 0.0f, billboardAngleY, 0.0f });
    XMFLOAT4 currentRotation = m_transform->GetRotation();
    currentRotation = MiMath::Slerp(currentRotation, targetRotation, m_rotationSpeed * deltaTime);

    m_transform->SetRotation(currentRotation);
    m_currentAngleY = m_transform->GetEulerAngle().y;
}
