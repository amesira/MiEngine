//===================================================
// transform_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

#include <algorithm> // std::clamp

#include "component.h"

class TransformComponent : public Component {
private:
    DirectX::XMFLOAT3   m_position = { 0.0f,0.0f,0.0f };
    DirectX::XMVECTOR   m_rotation = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
    DirectX::XMFLOAT3   m_scaling = { 1.0f,1.0f,1.0f };

    DirectX::XMFLOAT3   m_prevPosition = { 0.0f,0.0f,0.0f };

public:
    void    SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }
    void    SetRotation(DirectX::XMVECTOR rotation) { m_rotation = rotation; }
    void    SetScaling(DirectX::XMFLOAT3 scaling) { m_scaling = scaling; }

    DirectX::XMFLOAT3   GetPosition()const { return m_position; }
    DirectX::XMVECTOR   GetRotation()const { return m_rotation; }
    DirectX::XMFLOAT3   GetScaling()const { return m_scaling; }

    // 前フレームの位置取得・設定
    void    SetPrevPosition(DirectX::XMFLOAT3 prevPosition){ m_prevPosition = prevPosition; }
    DirectX::XMFLOAT3   GetPrevPosition()const { return m_prevPosition; }

    // Euler角での操作
    void    SetEulerRotation(DirectX::XMFLOAT3 euler) {
        m_rotation = XMQuaternionRotationRollPitchYaw(
            euler.x, euler.y, euler.z
        );
        m_rotation = XMQuaternionNormalize(m_rotation);
    }
    DirectX::XMFLOAT3   GetEulerRotation()const {
        // 念のため正規化（安全策）
        DirectX::XMVECTOR q = XMQuaternionNormalize(m_rotation);

        // 成分取得
        float x = XMVectorGetX(q);
        float y = XMVectorGetY(q);
        float z = XMVectorGetZ(q);
        float w = XMVectorGetW(q);

        DirectX::XMFLOAT3 euler;

        // Pitch (X)
        {
            float sinp = 2.0f * (w * x + y * z);
            float cosp = 1.0f - 2.0f * (x * x + y * y);
            euler.x = std::atan2(sinp, cosp);
        }

        // Yaw (Y)
        {
            float siny = 2.0f * (w * y - z * x);
            siny = std::clamp(siny, -1.0f, 1.0f); // 数値誤差対策
            euler.y = std::asin(siny);
        }

        // Roll (Z)
        {
            float sinr = 2.0f * (w * z + x * y);
            float cosr = 1.0f - 2.0f * (y * y + z * z);
            euler.z = std::atan2(sinr, cosr);
        }

        return euler; // ラジアン
    }
};


#endif