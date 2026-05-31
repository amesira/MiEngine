//---------------------------------------------------
// camera_control_behavior.h
// 
// Author・Miu Kitamura
// Date  ・ 2026/04/08
//---------------------------------------------------
#ifndef CAMERA_CONTROL_BEHAVIOR_H
#define CAMERA_CONTROL_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"

#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class CameraComponent;
class RigidbodyComponent;

// カメラ制御用のBehaviorComponent
class CameraControlBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;
    CameraComponent* m_camera = nullptr;

    // === TPSカメラのパラメータ ===
    // 追従ターゲットのアドレス
    TransformComponent* m_targetTransform = nullptr;

    // 注視点のオフセット
    XMFLOAT3 m_lookAtOffset = { 0.0f, 0.0f, 0.0f };
    float m_lookAtHeight = 1.5f; // 注視点の高さ
    float m_followDistance = 15.0f; // カメラと注視点の距離

    // （フォーカス用）
    TransformComponent* m_focusTarget = nullptr;
    float m_focusWeight = 0.5f; // フォーカスの重み（0.0f～1.0f）

    // === TPSカメラの操作設定 ===
    float m_mouseSensitivity = 1.0f; // マウス感度

    float m_rotationSmoothTime = 0.1f; // 回転のスムーズ時間
    float m_positionSmoothTime = 0.1f; // 位置のスムーズ時間

    // === カメラの制御設定 ===
    float m_pitch = XMConvertToRadians(40.0f);
    float m_yaw = 0.0f;

    float m_maxPitch = XMConvertToRadians(50.0f);
    float m_minPitch = XMConvertToRadians(-5.0f);

    float m_targetPitch = 0.0f;
    float m_targetYaw = 0.0f;

    // === カメラシェイク設定 ===
    bool m_isShaking = false;

    float m_shakeFrequency = 35.0f; // シェイクの周波数

    // === SmoothDampの状態 ===
    float m_pitchVelocity = 0.0f;
    float m_yawVelocity = 0.0f;

    XMFLOAT3 m_cameraPositionVelocity = {};
    XMFLOAT3 m_cameraOffsetVelocity = {};

public:
    ~CameraControlBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

private:
    // カメラの基底ベクトルの構築
    void BuildCameraBasis(XMFLOAT3& outForward, XMFLOAT3& outRight) const;

    // カメラの注視点のターゲット値を計算
    XMFLOAT3 CalculateTargetAtPosition();
    // カメラ回転のターゲット値の入力による更新
    void UpdateTargetYawPitchFromInput(float deltaTime);

    // カメラ位置を計算
    XMFLOAT3 CalculateTargetCameraPosition(const XMFLOAT3& cameraForward, const XMFLOAT3& cameraRight, const XMFLOAT3& targetAtPosition);

};

#endif // CAMERA_CONTROL_BEHAVIOR_H
