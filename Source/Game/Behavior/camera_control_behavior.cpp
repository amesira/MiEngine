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
#include "Engine/System/Device/mi_fps.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"

void CameraControlBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
    m_camera = GetOwner()->GetComponent<CameraComponent>();

    m_camera->SetFov(80.0f);

    FindTarget();
}

void CameraControlBehavior::Update()
{
    if (!m_targetTransform || !m_targetRigidbody) return;

    float deltaTime = FPS_GetDeltaTime();

    XMFLOAT3 targetPosition = m_targetTransform->GetPosition();
    XMFLOAT3 targetVelocity = m_targetRigidbody->GetVelocity();
    float targetSpeed = MiMath::Length(targetVelocity);

    CameraBasis basis = BuildCameraBasis();

    float speedLerp = std::clamp(targetSpeed / 15.0f, 0.0f, 1.0f);

    // ターゲットの速度に基づいてカメラの自動回転を更新
    UpdateAutoYaw(targetVelocity, targetSpeed, deltaTime, basis);
    basis = BuildCameraBasis();

    // ターゲットの位置とカメラの基底ベクトルに基づいて、カメラの注視点と位置の目標値を計算
    CameraDesiredPositions desiredPositions = CalculateDesiredPositions(targetPosition, basis);

    float smoothTime = 0.3f + 0.7f * (1.0f - speedLerp);
    m_atSmoothState = SmoothDamp(m_atSmoothState, desiredPositions.atPosition, smoothTime, deltaTime);
    m_eyeSmoothState = SmoothDamp(m_eyeSmoothState, desiredPositions.eyePosition, smoothTime, deltaTime);

    // スムーズに追従した位置をカメラのTransformとCameraComponentに適用
    m_transform->SetPosition(m_eyeSmoothState.position);
    m_camera->SetAtPosition(m_atSmoothState.position);
}

void CameraControlBehavior::DrawComponentInspector()
{
    float followDistance = m_followDistance;
    if (ImGui::SliderFloat("Follow Distance", &followDistance, 5.0f, 30.0f)) {
        m_followDistance = followDistance;
    }

    float pitch = MiMath::RadToDeg(m_pitch);
    if (ImGui::SliderFloat("Pitch", &pitch, MiMath::RadToDeg(m_minPitch), MiMath::RadToDeg(m_maxPitch))) {
        m_pitch = XMConvertToRadians(pitch);
    }

    float yaw = MiMath::RadToDeg(m_yaw);
    if (ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f)) {
        m_yaw = XMConvertToRadians(yaw);
    }
}

// ------------------------------- private

// ターゲットの探索
void CameraControlBehavior::FindTarget()
{
    IScene* scene = GetOwner()->GetScene();
    if (!scene) return;

    GameObject* target = scene->GetGameObjectByName("Player");
    if (!target) return;

    m_targetTransform = target->GetComponent<TransformComponent>();
    m_targetRigidbody = target->GetComponent<RigidbodyComponent>();
}

// ---------- 

// カメラの前方と右方向のベクトルを構築
CameraControlBehavior::CameraBasis CameraControlBehavior::BuildCameraBasis() const
{
    XMFLOAT4 cameraQuaternion = MiMath::QuaternionFromEuler({ m_pitch, m_yaw, 0.0f });

    CameraBasis basis{};
    basis.forward = MiMath::RotateVector(cameraQuaternion, { 0.0f, 0.0f, 1.0f });
    basis.right = MiMath::RotateVector(cameraQuaternion, { 1.0f, 0.0f, 0.0f });
    return basis;
}

// ターゲットの速度に基づいてカメラの自動回転を更新
void CameraControlBehavior::UpdateAutoYaw(const XMFLOAT3& targetVelocity, float targetSpeed, float deltaTime, const CameraBasis& basis)
{
    float autoWeight = std::clamp(targetSpeed / 10.0f, 0.0f, 1.0f);
    XMFLOAT3 flatCameraForward = MiMath::Normalize(XMFLOAT3(basis.forward.x, 0.0f, basis.forward.z));
    XMFLOAT3 flatTargetVelocity = MiMath::Normalize(XMFLOAT3(targetVelocity.x, 0.0f, targetVelocity.z));

    if (autoWeight > 0.01f && MiMath::Length(flatTargetVelocity) > 0.1f) {
        float yawDiffAngle = MiMath::Angle(flatCameraForward, flatTargetVelocity);
        if (yawDiffAngle > XMConvertToRadians(1.0f) && yawDiffAngle < XMConvertToRadians(120.0f)) {
            float sign = (MiMath::Cross(flatCameraForward, flatTargetVelocity).y < 0.0f) ? 1.0f : -1.0f;
            m_yaw += -yawDiffAngle * deltaTime * autoWeight * 0.5f * sign;
        }
    }
}

// カメラの注視点と位置の目標値を計算
CameraControlBehavior::CameraDesiredPositions CameraControlBehavior::CalculateDesiredPositions(const XMFLOAT3& targetPosition, const CameraBasis& basis) const
{
    CameraDesiredPositions desiredPositions{};

    desiredPositions.atPosition = MiMath::Add(targetPosition, XMFLOAT3{ 0.0f, m_followHeight, 0.0f });

    desiredPositions.eyePosition = MiMath::Subtract(
        desiredPositions.atPosition,
        MiMath::Multiply(basis.forward, m_followDistance));
    desiredPositions.eyePosition = MiMath::Add(
        desiredPositions.eyePosition,
        MiMath::Multiply(basis.right, 0.6f));

    return desiredPositions;
}

// -----------------

// スムーズダンピング関数の実装
CameraControlBehavior::CameraSmoothState CameraControlBehavior::SmoothDamp(
    const CameraSmoothState& current, const XMFLOAT3& targetPosition, float smoothTime, float deltaTime)
{
    if (1e-4f > smoothTime) {
        smoothTime = 1e-4f;
    }
    float omega = 2.0f / smoothTime;

    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    XMFLOAT3 change = MiMath::Subtract(current.position, targetPosition);
    XMFLOAT3 temp = MiMath::Add(current.velocity, MiMath::Multiply(change, omega));
    temp = MiMath::Multiply(temp, deltaTime);

    CameraSmoothState result;
    result.position = MiMath::Multiply(MiMath::Add(change, temp), exp);
    result.position = MiMath::Add(result.position, targetPosition);
    result.velocity = MiMath::Subtract(current.velocity, MiMath::Multiply(temp, omega));
    result.velocity = MiMath::Multiply(result.velocity, exp);
    return result;
}
