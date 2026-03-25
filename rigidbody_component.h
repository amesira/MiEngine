//===================================================
// rigidbody_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#ifndef RIGIDBODY_COMPONENT_H
#define RIGIDBODY_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

#include "component.h"

class RigidbodyComponent : public Component {
private:
    float               m_mass = 1.0f;
    float               m_gravityScale = -9.8f;

    bool                m_isKinematic = false;

    DirectX::XMFLOAT3   m_velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3   m_friction = { 0.98f, 1.0f, 0.98f };

    bool                m_isGrounded = false;
    DirectX::XMFLOAT3   m_prevVelocity = { 0.0f,0.0f,0.0f };

public:
    // 質量の設定・取得
    void    SetMass(float mass) { m_mass = mass; }
    float   GetMass() const { return m_mass; }
    // 重力の設定・取得
    void    SetGravityScale(float gravityScale) { m_gravityScale = gravityScale; }
    float   GetGravityScale() const { return m_gravityScale; }
    // キネマティックの設定・取得
    void    SetIsKinematic(bool isKinematic) { m_isKinematic = isKinematic; }
    bool    GetIsKinematic() const { return m_isKinematic; }

    // 速度の設定・取得
    void    SetVelocity(XMFLOAT3 velocity) { m_velocity = velocity; }
    XMFLOAT3   GetVelocity() const { return m_velocity; }
    // 摩擦の設定・取得
    void    SetFriction(XMFLOAT3 friction) { m_friction = friction; }
    XMFLOAT3   GetFriction() const { return m_friction; }

    // 地面接地フラグの設定・取得
    void    SetIsGrounded(bool isGrounded) { m_isGrounded = isGrounded; }
    bool    GetIsGrounded() const { return m_isGrounded; }

    // 前フレームの速度の設定・取得
    void    SetPrevVelocity(XMFLOAT3 prevVelocity) { m_prevVelocity = prevVelocity; }
    XMFLOAT3   GetPrevVelocity() const { return m_prevVelocity; }
};

#endif