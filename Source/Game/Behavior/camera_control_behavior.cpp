//===================================================
// camera_control_behavior.cpp
// 
// Author・Miu Kitamura
// Date  ・ 2026/04/08
//===================================================
#include "camera_control_behavior.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Utility/mi_math.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Device/mouse.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"

void CameraControlBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
    m_camera = GetOwner()->GetComponent<CameraComponent>();

    IScene* scene = GetOwner()->GetScene();

    // ターゲットのTransformComponentの参照取得
    {
        GameObject* target = scene->GetGameObjectByName("Player");
        if (target) {
            m_targetTransform = target->GetComponent<TransformComponent>();
        }
    }
}

void CameraControlBehavior::Update()
{
    if (!m_targetTransform) return;
    float deltaTime = FPS_GetUnscaledDeltaTime();

    // マウス入力から回転のターゲット値を更新
    UpdateTargetYawPitchFromInput(deltaTime);
    m_targetPitch = MiMath::Clamp(m_targetPitch, m_minPitch, m_maxPitch);

    // カメラ位置のターゲット値を計算
    XMFLOAT3 targetAtPosition = CalculateTargetAtPosition();

    // === カメラの回転と位置のスムーズ追従 ===
    // カメラの回転をスムーズに追従
    m_pitch = MiMath::SmoothDamp(m_pitch, m_targetPitch, m_pitchVelocity, m_rotationSmoothTime, deltaTime);
    m_yaw = MiMath::SmoothDamp(m_yaw, m_targetYaw, m_yawVelocity, m_rotationSmoothTime, deltaTime);

    // カメラの注視点をスムーズに追従
    XMFLOAT3 currentCameraAtPosition = m_camera->GetAtPosition();
    currentCameraAtPosition = MiMath::SmoothDamp(currentCameraAtPosition, targetAtPosition, m_cameraPositionVelocity, m_positionSmoothTime, deltaTime);
    
    // カメラ位置を計算
    XMFLOAT3 cameraForward, cameraRight;
    BuildCameraBasis(cameraForward, cameraRight);

    XMFLOAT3 targetCameraPosition = CalculateTargetCameraPosition(cameraForward, cameraRight, currentCameraAtPosition);
    XMFLOAT3 currentCameraPosition = m_transform->GetPosition();
    currentCameraPosition = MiMath::SmoothDamp(currentCameraPosition, targetCameraPosition, m_cameraOffsetVelocity, m_positionSmoothTime, deltaTime);

    // === カメラの適用処理 ===
    m_camera->SetAtPosition(currentCameraAtPosition);
    m_transform->SetPosition(currentCameraPosition);
}

// ImGuiを使ったインスペクタの描画
void CameraControlBehavior::DrawComponentInspector()
{
    float followDistance = m_followDistance;
    if (ImGui::SliderFloat("Follow Distance", &followDistance, 5.0f, 30.0f)) {
        m_followDistance = followDistance;
    }
    float lookAtHeight = m_lookAtHeight;
    if (ImGui::SliderFloat("LookAt Height", &lookAtHeight, 0.0f, 5.0f)) {
        m_lookAtHeight = lookAtHeight;
    }

    float targetPitchDegrees = XMConvertToDegrees(m_targetPitch);
    if (ImGui::SliderFloat("Target Pitch", &targetPitchDegrees, -90.0f, 90.0f)) {
        m_targetPitch = XMConvertToRadians(targetPitchDegrees);
    }
    float targetYawDegrees = XMConvertToDegrees(m_targetYaw);
    if (ImGui::SliderFloat("Target Yaw", &targetYawDegrees, -180.0f, 180.0f)) {
        m_targetYaw = XMConvertToRadians(targetYawDegrees);
    }
}

// ------------------------------- private

// カメラの前方と右方向のベクトルを構築
void CameraControlBehavior::BuildCameraBasis(XMFLOAT3& outForward, XMFLOAT3& outRight) const
{
    XMFLOAT4 cameraQuaternion = MiMath::QuaternionFromEuler({ m_pitch, m_yaw, 0.0f });

    outForward = MiMath::RotateVector(cameraQuaternion, { 0.0f, 0.0f, 1.0f });
    outRight = MiMath::RotateVector(cameraQuaternion, { 1.0f, 0.0f, 0.0f });
}

// カメラの注視点の目標値を計算
XMFLOAT3 CameraControlBehavior::CalculateTargetAtPosition()
{
    // ターゲットの位置を取得
    XMFLOAT3 targetPosition = m_targetTransform->GetPosition();
    if (m_focusTarget) {
        XMFLOAT3 focusPosition = m_focusTarget->GetPosition();
        targetPosition = MiMath::Lerp(targetPosition, focusPosition, m_focusWeight);
    }
    targetPosition = MiMath::Add(targetPosition, m_lookAtOffset);
    targetPosition.y += m_lookAtHeight;

    return targetPosition;
}

// カメラ回転のターゲット値の入力による更新
void CameraControlBehavior::UpdateTargetYawPitchFromInput(float deltaTime)
{
    // マウス入力から回転のターゲット値を計算
    float mouseX = Mouse_GetPositionX() - Mouse_GetOldPositionX();
    float mouseY = Mouse_GetPositionY() - Mouse_GetOldPositionY();

    m_targetYaw += mouseX * m_mouseSensitivity * deltaTime;
    m_targetPitch += mouseY * m_mouseSensitivity * deltaTime;
}

// カメラ位置のターゲット値を計算
XMFLOAT3 CameraControlBehavior::CalculateTargetCameraPosition(const XMFLOAT3& cameraForward, const XMFLOAT3& cameraRight, const XMFLOAT3& targetAtPosition)
{
    XMFLOAT3 targetCameraPosition = targetAtPosition;
    targetCameraPosition = MiMath::Subtract(targetCameraPosition, MiMath::Multiply(cameraForward, m_followDistance));

    return targetCameraPosition;
}
