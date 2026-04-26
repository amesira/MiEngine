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

    // 追従ターゲットのアドレス
    TransformComponent* m_targetTransform = nullptr;
    RigidbodyComponent* m_targetRigidbody = nullptr;

    // カメラの回転角度
    float m_pitch = XMConvertToRadians(40.0f);
    float m_yaw = 0.0f;

    float m_maxPitch = XMConvertToRadians(50.0f);
    float m_minPitch = XMConvertToRadians(-5.0f);

    // カメラの距離と高さ
    float m_followDistance = 20.0f;
    float m_followHeight = 2.0f;

    // カメラ追従のスムージングパラメータ
    struct CameraSmoothState {
        XMFLOAT3 position;
        XMFLOAT3 velocity;
    };

    struct CameraBasis {
        XMFLOAT3 forward;
        XMFLOAT3 right;
    };

    struct CameraDesiredPositions {
        XMFLOAT3 atPosition;
        XMFLOAT3 eyePosition;
    };

    CameraSmoothState m_atSmoothState;
    CameraSmoothState m_eyeSmoothState;

public:
    ~CameraControlBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

private:
    // ターゲットの探索
    void FindTarget();
    // カメラの基底ベクトルの構築
    CameraBasis BuildCameraBasis() const;

    // ターゲットの速度に基づいてカメラの自動回転を更新
    void UpdateAutoYaw(const XMFLOAT3& targetVelocity, float targetSpeed, float deltaTime, const CameraBasis& basis);
    // ターゲットの位置とカメラの基底ベクトルに基づいて、カメラの注視点と位置の目標値を計算
    CameraDesiredPositions CalculateDesiredPositions(const XMFLOAT3& targetPosition, const CameraBasis& basis) const;
    
    // スムーズダンピング関数
    CameraSmoothState SmoothDamp(const CameraSmoothState& current, const XMFLOAT3& targetPosition, float smoothTime, float deltaTime);
};

#endif // CAMERA_CONTROL_BEHAVIOR_H
