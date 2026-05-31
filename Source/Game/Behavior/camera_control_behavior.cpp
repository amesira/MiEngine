//===================================================
// camera_control_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/08
//===================================================
#include "camera_control_behavior.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include <algorithm>

#include "Utility/mi_math.h"
#include "Utility/mi_signal.h"

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

    // 初期設定
    m_targetPitch = m_pitch;
    m_targetYaw = m_yaw;

    // デフォルト値の保存
    m_defaultLookAtOffset = m_lookAtOffset;
    m_defaultLookAtOffset = m_lookAtOffset;
    m_defaultFollowDistance = m_followDistance;
    if (m_camera) {
        m_defaultFov = m_camera->GetFov();
        m_fovTask.m_currentValue = m_defaultFov;
    }

    // タスクのリセット
    m_cameraDistanceTask.m_currentValue = m_followDistance;
    m_cameraOffsetTask.m_currentValue = m_lookAtOffset;
    m_fovTask.m_endValue = m_defaultFov;
    m_cameraDistanceTask.m_endValue = m_defaultFollowDistance;
    m_cameraOffsetTask.m_endValue = m_defaultLookAtOffset;

    m_fovTask.Reset();
    m_cameraDistanceTask.Reset();
    m_cameraOffsetTask.Reset();
}

void CameraControlBehavior::Update()
{
    float deltaTime = FPS_GetUnscaledDeltaTime();
    UpdateCameraEffectTasks(deltaTime);

    if (!m_targetTransform) return;

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

    // === カメラシェイクの適用 ===
    if (m_isShaking) {
        currentCameraAtPosition = MiMath::Add(currentCameraAtPosition, m_shakeOffset);
        currentCameraPosition = MiMath::Add(currentCameraPosition, m_shakeOffset);
    }

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

// ------------------------------- public Effect Tasks

void CameraControlBehavior::ChangeFOV(float fov, float duration)
{
    if (!m_camera) return;

    m_fovTask.Reset();

    m_fovTask.m_startValue = m_camera->GetFov();
    m_fovTask.m_targetValue = fov;
    m_fovTask.m_endValue = fov;
    m_fovTask.m_duration = duration;
    m_fovTask.m_holdDuration = 0.0f;
    m_fovTask.Start();
}

void CameraControlBehavior::ChangeFOVTemporary(float fov, float duration, float holdDuration)
{
    if (!m_camera) return;

    m_fovTask.Reset();

    m_fovTask.m_startValue = m_camera->GetFov();
    m_fovTask.m_targetValue = fov;
    m_fovTask.m_endValue = m_defaultFov;
    m_fovTask.m_duration = duration;
    m_fovTask.m_holdDuration = holdDuration;
    m_fovTask.Start();
}

void CameraControlBehavior::ResetFOV(float duration)
{
    ChangeFOV(m_defaultFov, duration);
}

void CameraControlBehavior::ChangeCameraDistance(float distance, float duration)
{
    m_cameraDistanceTask.Reset();

    m_cameraDistanceTask.m_startValue = m_followDistance;
    m_cameraDistanceTask.m_targetValue = distance;
    m_cameraDistanceTask.m_endValue = distance;
    m_cameraDistanceTask.m_duration = duration;
    m_cameraDistanceTask.m_holdDuration = 0.0f;
    m_cameraDistanceTask.Start();
}

void CameraControlBehavior::ChangeCameraDistanceTemporary(float distance, float duration, float holdDuration)
{
    m_cameraDistanceTask.Reset();
    m_cameraDistanceTask.m_startValue = m_followDistance;
    m_cameraDistanceTask.m_targetValue = distance;
    m_cameraDistanceTask.m_endValue = m_defaultFollowDistance;  // 変更後の距離からデフォルトの距離に戻るように設定
    m_cameraDistanceTask.m_duration = duration;
    m_cameraDistanceTask.m_holdDuration = holdDuration;
    m_cameraDistanceTask.Start();
}

void CameraControlBehavior::ResetCameraDistance(float duration)
{
    ChangeCameraDistance(m_defaultFollowDistance, duration);
}

void CameraControlBehavior::ChangeCameraOffset(const XMFLOAT3& offset, float duration)
{
    m_cameraOffsetTask.Reset();

    m_cameraOffsetTask.m_startValue = m_lookAtOffset;
    m_cameraOffsetTask.m_targetValue = offset;
    m_cameraOffsetTask.m_endValue = offset;
    m_cameraOffsetTask.m_duration = duration;
    m_cameraOffsetTask.m_holdDuration = 0.0f;
    m_cameraOffsetTask.Start();
}

void CameraControlBehavior::ChangeCameraOffsetTemporary(const XMFLOAT3& offset, float duration, float holdDuration)
{
    m_cameraOffsetTask.Reset();

    m_cameraOffsetTask.m_startValue = m_lookAtOffset;
    m_cameraOffsetTask.m_targetValue = offset;
    m_cameraOffsetTask.m_endValue = m_defaultLookAtOffset;  // 変更後のオフセットからデフォルトのオフセットに戻るように設定
    m_cameraOffsetTask.m_duration = duration;
    m_cameraOffsetTask.m_holdDuration = holdDuration;
    m_cameraOffsetTask.Start();
}

void CameraControlBehavior::ResetCameraOffset(float duration)
{
    ChangeCameraOffset(m_defaultLookAtOffset, duration);
}

void CameraControlBehavior::PlayCameraShake(float duration, float magnitude)
{
    m_cameraShakeTask.Reset();
    m_cameraShakeTask.m_duration = duration;
    m_cameraShakeTask.m_magnitude = magnitude;
    m_cameraShakeTask.m_shakeFrequency = m_shakeFrequency; // シェイクの周波数はクラスの設定値を使用
    m_cameraShakeTask.Start();
}

// ------------------------------- private

void CameraControlBehavior::UpdateCameraEffectTasks(float deltaTime)
{
    //タスクが実行中かどうか
    bool fovTaskRunning = !m_fovTask.IsFinished();
    bool distanceTaskRunning = !m_cameraDistanceTask.IsFinished();
    bool offsetTaskRunning = !m_cameraOffsetTask.IsFinished();
    bool cameraShakeTaskRunning = !m_cameraShakeTask.IsFinished();

    // タスクの更新
    m_fovTask.Update(deltaTime);
    m_cameraDistanceTask.Update(deltaTime);
    m_cameraOffsetTask.Update(deltaTime);
    m_cameraShakeTask.Update(deltaTime);

    // タスクの更新後に値を適用
    if (fovTaskRunning && m_camera) {
        m_camera->SetFov(m_fovTask.m_currentValue);
    }
    if (distanceTaskRunning) {
        m_followDistance = m_cameraDistanceTask.m_currentValue;
    }
    if (offsetTaskRunning) {
        m_lookAtOffset = m_cameraOffsetTask.m_currentValue;
    }
    if (cameraShakeTaskRunning) {
        m_isShaking = true;
        m_shakeOffset = m_cameraShakeTask.m_shakeOffset;
    }
    else {
        m_isShaking = false;
    }
}

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

// カメラシェイクタスクの更新
void CameraControlBehavior::CameraShakeTask::Update(float deltaTime)
{
    SequenceTask::Update(deltaTime);
    if (IsFinished()) return;

    switch (m_taskStep) {
        case 0: {
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }

            float elapsed = m_taskTimer;

            // シェイクの強さを時間経過に応じて減衰させる
            m_magnitude = MiMath::Lerp(m_magnitude, 0.0f, t * t);

            // Perlinノイズを使用してシェイクのオフセットを生成
            m_shakeOffset = { 0.0f, 0.0f, 0.0f };
            m_shakeOffset.x = MiSignal::Perlin1D(elapsed * m_shakeFrequency) * m_magnitude;
            m_shakeOffset.y = MiSignal::Perlin1D((elapsed + 100.0f) * m_shakeFrequency) * m_magnitude;

            // タスクの終了判定
            if (m_taskTimer >= m_duration) {
                Finish();
            }
            break;
        }
    }
}
