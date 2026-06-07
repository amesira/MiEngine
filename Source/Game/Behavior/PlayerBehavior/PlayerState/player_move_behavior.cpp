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

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"

#include <cmath>

void PlayerMoveBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;

    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_spriteRenderer = owner->GetComponent<SpriteRendererComponent>();

    IScene* scene = owner->GetScene();

    // メインカメラの参照取得
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

// ----------------------------------------------- public

// 移動更新処理
void PlayerMoveBehavior::UpdateMove(const PlayerContext& context, const PlayerMoveRequest& moveRequest, float deltaTime)
{
    if (!m_rigidbody) return;

    const PlayerInput& input = context.input;
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();

    // 移動できない場合は速度を0にして終了
    if (!moveRequest.canMove) {
        velocity.x = 0.0f;
        velocity.z = 0.0f;
        m_rigidbody->SetVelocity(velocity);
        return;
    }

    XMFLOAT3 moveDirection = { input.moveInputCameraLocal.x, 0.0f, input.moveInputCameraLocal.z };
    moveDirection = MiMath::Multiply(MiMath::Normalize(moveDirection), m_moveSpeed);

    velocity.x += moveDirection.x * deltaTime * m_acceleration * moveRequest.speedMultiplier;
    velocity.z += moveDirection.z * deltaTime * m_acceleration * moveRequest.speedMultiplier;

    velocity.x = MiMath::Clamp(velocity.x, -m_moveSpeed * moveRequest.speedMultiplier, m_moveSpeed * moveRequest.speedMultiplier);
    velocity.z = MiMath::Clamp(velocity.z, -m_moveSpeed * moveRequest.speedMultiplier, m_moveSpeed * moveRequest.speedMultiplier);

    // ジャンプ処理
    if (input.triggerJumpCommand && m_rigidbody->GetIsGrounded()) {
        velocity.y = m_jumpForce;
    }

    m_rigidbody->SetVelocity(velocity);
}

// 回転更新処理
void PlayerMoveBehavior::UpdateRotation(const PlayerContext& context, const PlayerMoveRequest& moveRequest, float deltaTime)
{
    if (!moveRequest.canRotate) return;
    if (moveRequest.rotationMode == PlayerRotationMode::Locked) return;
    if (!m_transform || !m_mainCamera) return;

    // カメラ正面を取得
    XMFLOAT3 forward = m_mainCamera->GetForward();

    switch (moveRequest.rotationMode) {
    case PlayerRotationMode::CameraForward: // === 常にカメラ正面を向く ===
        if (context.input.horizontal > 0.01f) {
            m_spriteRenderer->SetFlipX(true);
        }
        else if (context.input.horizontal < -0.01f) {
            m_spriteRenderer->SetFlipX(false);
        }
        break;
    case PlayerRotationMode::AimForward: // === エイム状態 ===
        forward = MiMath::Add(forward, MiMath::Multiply(m_mainCamera->GetRight(), -1.0f));
        forward = MiMath::Normalize(forward);
        m_spriteRenderer->SetFlipX(true);
        break;
    default:
        break;
    }

    // 正面方向からY軸回転の角度を計算
    const float billboardAngleY = atan2f(forward.x, forward.z);
    XMFLOAT4 targetRotation = MiMath::QuaternionFromEuler({ 0.0f, billboardAngleY, 0.0f });
    XMFLOAT4 currentRotation = m_transform->GetRotation();
    currentRotation = MiMath::Slerp(currentRotation, targetRotation, m_rotationSpeed * deltaTime);

    m_transform->SetRotation(currentRotation);
    m_currentAngleY = m_transform->GetEulerAngle().y;
}
