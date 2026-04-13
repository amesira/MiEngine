//===================================================
// camera_control_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/08
//===================================================
#include "camera_control_behavior.h"
#include "scene_interface.h"
#include "game_object.h"

#include "mi_math.h"
#include "mi_fps.h"

#include "imgui_window_interface.h"

#include "transform_component.h"
#include "camera_component.h"
#include "rigidbody_component.h"

// カメラ制御の初期化処理
void CameraControlBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
    m_camera = GetOwner()->GetComponent<CameraComponent>();

    // 追従ターゲットのTransformComponentとRigidbodyComponentを取得
    IScene* scene = GetOwner()->GetScene();
    if (scene) {
        GameObject* target = scene->GetGameObjectByName("Player"); // 追従ターゲットの名前を指定
        if (target) {
            m_targetTransform = target->GetComponent<TransformComponent>();
            m_targetRigidbody = target->GetComponent<RigidbodyComponent>();
        }
    }
}

// カメラ制御の更新処理
void CameraControlBehavior::Update()
{
    if (!m_targetTransform || !m_targetRigidbody) return;

    float deltaTime = FPS_GetDeltaTime();

    XMFLOAT3 targetPosition = m_targetTransform->GetPosition();
    XMFLOAT3 targetVelocity = m_targetRigidbody->GetVelocity();
    float targetSpeed = MiMath::Length(targetVelocity);

    // 速度に応じたスムージングの調整
    float speedLerp = std::clamp(targetSpeed / 15.0f, 0.0f, 1.0f);

    // 自動回転ウェイト
    float autoWeight = std::clamp(targetSpeed / 10.0f, 0.0f, 1.0f);

    XMFLOAT4 cameraQuaternion = MiMath::QuaternionFromEuler({ m_pitch, m_yaw, 0.0f });
    XMFLOAT3 cameraForward = MiMath::RotateVector(cameraQuaternion, {0.0f, 0.0f, 1.0f});
    XMFLOAT3 cameraRight = MiMath::RotateVector(cameraQuaternion, { 1.0f, 0.0f, 0.0f });

    // 注視点の目標位置を計算
    XMFLOAT3 desiredAtPosition = MiMath::Add(targetPosition, XMFLOAT3{ 0.0f, m_followHeight, 0.0f });
    
    // カメラ位置の目標位置を計算
    XMFLOAT3 desiredEyePosition = MiMath::Subtract(desiredAtPosition, MiMath::Multiply(cameraForward, m_followDistance));
    desiredEyePosition = MiMath::Add(desiredEyePosition, MiMath::Multiply(cameraRight, 0.6f)); // 少し右にオフセット（マジックナンバー要修正）

    // 注視点とカメラ位置をスムーズダンパーで更新
    m_atSmoothState = SmoothDamp(m_atSmoothState, desiredAtPosition, 0.3f + 0.7f * (1.0f - speedLerp), deltaTime);
    m_eyeSmoothState = SmoothDamp(m_eyeSmoothState, desiredEyePosition, 0.3f + 0.7f * (1.0f - speedLerp), deltaTime);

    // カメラコンポーネントに位置と注視点を設定
    m_transform->SetPosition(m_eyeSmoothState.position);
    m_camera->SetAtPosition(m_atSmoothState.position);
}

// カメラ制御のインスペクタ表示処理
void CameraControlBehavior::DrawComponentInspector()
{
    float followDistance = m_followDistance;
    if (ImGui::SliderFloat("Follow Distance", &followDistance, 5.0f, 30.0f)) {
        m_followDistance = followDistance;
    }
}

// スムーズダンパーの実装（CameraSmoothState版）
CameraControlBehavior::CameraSmoothState CameraControlBehavior::SmoothDamp(
    const CameraSmoothState& current, const XMFLOAT3& targetPosition, float smoothTime, float deltaTime)
{
    if (1e-4f > smoothTime) {
        smoothTime = 1e-4f;
    }
    float omega = 2.0f / smoothTime;

    // オイラー法で近似的に減衰を計算
    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    // 目標位置と現在位置の差分を計算
    XMFLOAT3 change = MiMath::Subtract(current.position, targetPosition);
    XMFLOAT3 temp = MiMath::Add(current.velocity, MiMath::Multiply(change, omega));
    temp = MiMath::Multiply(temp, deltaTime);

    // 新しい位置と速度を計算
    CameraSmoothState result;
    result.position = MiMath::Multiply(MiMath::Add(change, temp), exp);
    result.position = MiMath::Add(result.position, targetPosition);
    result.velocity = MiMath::Subtract(current.velocity, MiMath::Multiply(temp, omega));
    result.velocity = MiMath::Multiply(result.velocity, exp);
    return result;
}
