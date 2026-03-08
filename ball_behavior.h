//---------------------------------------------------
// ball_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/08
//---------------------------------------------------
#ifndef BALL_BEHAVIOR_H
#define BALL_BEHAVIOR_H
#include "behavior.h"

#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class RigidbodyComponent;
class SphereColliderComponent;

class BallBehavior : public Behavior{
private:
    TransformComponent* m_transform = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;
    SphereColliderComponent* m_collider = nullptr;

    XMFLOAT3 m_moveDirection = { 0.0f,0.0f,0.0f };
    XMFLOAT3 m_bounceVelocity = { 0.0f,0.0f,0.0f };

    float   m_destroyTimer = 0.0f;

    bool m_isGrounded = false;

    float m_radius = 0.5f;

public:
    BallBehavior(GameObject* owner);
    ~BallBehavior();

    void    Update(IScene* pScene) override;

private:
    // ボール回転の更新
    void    UpdateBallRotation(float deltaTime);
    // 物体破壊の更新
    void    UpdateBreakObjects();

public:
    // 移動方向設定
    void    SetMoveDirection(XMFLOAT3 dir) { m_moveDirection = dir; }
    // 跳ね返り速度追加
    void    AddBounceVelocity(XMFLOAT3 velocity) {
        m_bounceVelocity.x += velocity.x;
        m_bounceVelocity.y += velocity.y;
        m_bounceVelocity.z += velocity.z;
    }

    XMFLOAT3   GetPosition() const;
    void    SetDestroyTimer(float time) { m_destroyTimer = time; }

    bool    IsGrounded() const { return m_isGrounded; }

    void    AddRadius(float deltaRadius);

};
#endif // BALL_BEHAVIOR_H