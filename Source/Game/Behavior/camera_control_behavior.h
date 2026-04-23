//---------------------------------------------------
// camera_control_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/08
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
    TransformComponent* m_transform;
    CameraComponent*    m_camera;

    // 追従ターゲットのアドレス
    TransformComponent* m_targetTransform;
    RigidbodyComponent* m_targetRigidbody;

    //-----------------

    // カメラの回転角度
    float m_pitch = XMConvertToRadians(30.0f); // カメラのピッチ角
    float m_yaw = 0.0f;   // カメラのヨー角

    float m_maxPitch = XMConvertToRadians(40.0f); // ピッチの最大角度
    float m_minPitch = XMConvertToRadians(-5.0f); // ピッチの最小角度

    // カメラの距離と高さ
    float m_followDistance = 15.0f; // ターゲットからの距離
    float m_followHeight = 2.0f;   // ターゲットからの高さ

    // カメラ追従のスムージングパラメーター
    struct CameraSmoothState {
        XMFLOAT3 position;
        XMFLOAT3 velocity;
    };
    CameraSmoothState m_atSmoothState;  // 注視点のスムーズダンパー状態
    CameraSmoothState m_eyeSmoothState; // カメラ位置のスムーズダンパー状態

public:
    ~CameraControlBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

private:
    // スムーズダンパー
    CameraSmoothState SmoothDamp(const CameraSmoothState& current, const XMFLOAT3& targetPosition, float smoothTime, float deltaTime);

public:

};

#endif // CAMERA_CONTROL_BEHAVIOR_H