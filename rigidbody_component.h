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

#include "component.h"

class RigidbodyComponent : public Component {
private:
    float               m_mass = 1.0f;
    float               m_gravityScale = -9.8f;

    DirectX::XMFLOAT3   m_velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3   m_friction = { 0.98f, 1.0f, 0.98f };

    bool                m_isGrounded = false;
    DirectX::XMFLOAT3   m_prevVelocity = { 0.0f,0.0f,0.0f };

public:
    void    SetMass(float mass) { m_mass = mass; }
    void    SetGravityScale(float gravityScale) { m_gravityScale = gravityScale; }
    void    SetVelocity(DirectX::XMFLOAT3 velocity) { m_velocity = velocity; }
    void    SetFriction(DirectX::XMFLOAT3 friction) { m_friction = friction; }

    float               GetMass() const { return m_mass; }
    float               GetGravityScale() const { return m_gravityScale; }
    DirectX::XMFLOAT3   GetVelocity() const { return m_velocity; }
    DirectX::XMFLOAT3   GetFriction() const { return m_friction; }

    void    SetIsGrounded(bool isGrounded) { m_isGrounded = isGrounded; }
    void    SetPrevVelocity(DirectX::XMFLOAT3 prevVelocity) { m_prevVelocity = prevVelocity; }
    
    bool                GetIsGrounded() const { return m_isGrounded; }
    DirectX::XMFLOAT3   GetPrevVelocity() const { return m_prevVelocity; }
};

#endif