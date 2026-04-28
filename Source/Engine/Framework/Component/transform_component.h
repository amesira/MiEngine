//===================================================
// transform_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H
#include "Engine/Core/component.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

#include <algorithm>
#include "Utility/mi_math.h"

class TransformComponent : public Component {
private:
    // 位置、回転、スケーリングのデータ
    XMFLOAT3   m_position = { 0.0f,0.0f,0.0f };
    XMFLOAT4   m_rotation = { 0.0f,0.0f,0.0f,1.0f };
    XMFLOAT3   m_scaling = { 1.0f,1.0f,1.0f };

    // 前フレームの位置を保存する変数
    XMFLOAT3   m_prevPosition = { 0.0f,0.0f,0.0f };

public:
    // 位置、回転、スケーリングの設定・取得
    void    SetPosition(const XMFLOAT3& position) { m_position = position; }
    XMFLOAT3   GetPosition()const { return m_position; }
    void    SetRotation(const XMFLOAT4& rotation) { m_rotation = rotation; }
    XMFLOAT4   GetRotation()const { return m_rotation; }
    void    SetScaling(const XMFLOAT3& scaling) { m_scaling = scaling; }
    XMFLOAT3   GetScaling()const { return m_scaling; }

    // 前フレームの位置取得・設定
    void    SetPrevPosition(const XMFLOAT3& prevPosition){ m_prevPosition = prevPosition; }
    XMFLOAT3   GetPrevPosition()const { return m_prevPosition; }

    // Forward、Right、Upの取得
    XMFLOAT3   GetForward() const {
        XMFLOAT3 forward = { 0.0f, 0.0f, 1.0f };
        forward = MiMath::RotateVector(m_rotation, forward);
        return forward;
    }
    XMFLOAT3   GetRight() const {
        XMFLOAT3 right = { 1.0f, 0.0f, 0.0f };
        right = MiMath::RotateVector(m_rotation, right);
        return right;
    }
    XMFLOAT3   GetUp() const {
        XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
        up = MiMath::RotateVector(m_rotation, up);
        return up;
    }

    // Euler角での設定・取得（ラジアン角で扱う）
    void    SetEulerAngle(const XMFLOAT3& euler) {
        XMVECTOR q = XMQuaternionRotationRollPitchYaw(
            euler.x, euler.y, euler.z
        );
        q = XMQuaternionNormalize(q);
        XMStoreFloat4(&m_rotation, q);
    }
    XMFLOAT3    GetEulerAngle()const {
        const XMFLOAT4& q = m_rotation;

        float sinX = 2.0f * q.y * q.z - 2.0f * q.x * q.w;
        const float e = 0.001f;

        // X軸の回転が0付近の場合は丸める
        if (std::abs(sinX) < e) {
            sinX = 0.0f;
        }

        float x = std::asinf(-sinX);
        float cosX = std::cosf(x);

        float sinY = (2.0f * q.x * q.z + 2.0f * q.y * q.w) / cosX;
        float cosY = (2.0f * MiMath::Pow(q.w, 2) + 2.0f * MiMath::Pow(q.z, 2) - 1.0f) / cosX;
        float y = std::atan2f(sinY, cosY);

        float sinZ = (2.0f * q.x * q.y + 2.0f * q.z * q.w) / cosX;
        float cosZ = (2.0f * MiMath::Pow(q.w, 2) + 2.0f * MiMath::Pow(q.y, 2) - 1.0f) / cosX;
        float z = std::atan2f(sinZ, cosZ);

        XMFLOAT3 euler = { x, y, z };
        return euler;
    }

    // XMVECTORの取得
    XMVECTOR GetRotationVector() const {
        return XMLoadFloat4(&m_rotation);
    }

};


#endif