//===================================================
// transform_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H
#include "component.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

#include <algorithm>
#include "mi_math.h"

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

        // ジンバルロック（X軸の回転が±90度付近）を回避するための処理
        /*if (std::isnan(x) || std::abs(std::abs(x) - XM_PI / 2.0f) < e) {
            x = -sinX > 0.0f ? XM_PI / 2.0f : -XM_PI / 2.0f;
            float y = 0.0f;
            if (x > 0.0f) {
                y = std::atan2f(2.0f * q.x * q.y - 2.0f * q.z * q.w, 2.0f * MiMath::Pow(q.w, 2) + 2.0f * MiMath::Pow(q.x, 2) - 1.0f);
            }
            else {
                y = std::atan2f(-(2.0f * q.x * q.y - 2.0f * q.z * q.w), 2.0f * MiMath::Pow(q.w, 2) + 2.0f * MiMath::Pow(q.x, 2) - 1.0f);
            }
            XMFLOAT3 zimbalLockEuler = { x, y, 0.0f };
        }*/

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