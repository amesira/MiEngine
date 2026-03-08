#pragma once
#include <DirectXMath.h>
using namespace DirectX;

namespace MiMath
{
    // 値をmin〜maxの範囲にクランプする
    inline float Clamp(float value, float min, float max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    // 累乗を計算する
    inline float Pow(float base, int exponent = 2) {
        float result = 1.0f;
        for (int i = 0; i < exponent; i++) {
            result *= base;
        }
        return result;
    }

    // 内積を計算する
    inline float Dot(const XMVECTOR& va, const XMVECTOR& vb) {
        return XMVectorGetX(XMVector3Dot(va, vb));
    }
    inline float Dot(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR va = XMLoadFloat3(&a);
        XMVECTOR vb = XMLoadFloat3(&b);
        return Dot(va, vb);
    }

    // 外積を計算する
    inline XMFLOAT3 Cross(const XMVECTOR& va, const XMVECTOR& vb) {
        XMFLOAT3 result = {};
        XMStoreFloat3(&result, XMVector3Cross(va, vb));
        return result;
    }
    inline XMFLOAT3 Cross(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR va = XMLoadFloat3(&a);
        XMVECTOR vb = XMLoadFloat3(&b);
        return Cross(va, vb);
    }

    // ２点間の距離を計算する
    inline float Distance(const XMVECTOR& va, const XMVECTOR& vb) {
        XMVECTOR diff = XMVectorSubtract(va, vb);
        return XMVectorGetX(XMVector3Length(diff));
    }
    inline float Distance(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR va = XMLoadFloat3(&a);
        XMVECTOR vb = XMLoadFloat3(&b);
        return Distance(va, vb);
    }

    // ベクトルの長さを計算する
    inline float Length(const XMVECTOR& vv) {
        return XMVectorGetX(XMVector3Length(vv));
    }
    inline float Length(const XMFLOAT3& v) {
        return Length(XMLoadFloat3(&v));
    }

    // ベクトルを正規化する
    inline XMFLOAT3 Normalize(const XMVECTOR& vv) {
        XMFLOAT3 v = {};
        XMStoreFloat3(&v, XMVector3Normalize(vv));
        return v;
    }
    inline XMFLOAT3 Normalize(const XMFLOAT3& v) {
        XMVECTOR vv = XMLoadFloat3(&v);
        return Normalize(vv);
    }

    // ２つのベクトル間の角度を計算する（ラジアン）
    inline float Angle(const XMVECTOR& from, const XMVECTOR& to) {
        XMVECTOR vf = XMVector3Normalize(from);
        XMVECTOR vt = XMVector3Normalize(to);
        float cosTheta = XMVectorGetX(XMVector3Dot(vf, vt));
        cosTheta = Clamp(cosTheta, -1.0f, 1.0f);
        return acosf(cosTheta);
    }
    inline float Angle(const XMFLOAT3& from, const XMFLOAT3& to) {
        XMVECTOR vf = XMLoadFloat3(&from);
        XMVECTOR vt = XMLoadFloat3(&to);
        return Angle(vf, vt);
    }

    inline float RadToDeg(float radian) {
        return radian * (180.0f / 3.14159265f);
    }

    // 線形補間
    inline XMVECTOR Lerp(const XMVECTOR& a, const XMVECTOR& b, float t) {
        return XMVectorAdd(a, XMVectorScale(XMVectorSubtract(b, a), t));
    }
    inline XMFLOAT3 Lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t) {
        XMVECTOR va = XMLoadFloat3(&a);
        XMVECTOR vb = XMLoadFloat3(&b);
        XMVECTOR vr = Lerp(va, vb, t);
        XMFLOAT3 result;
        XMStoreFloat3(&result, vr);
        return result;
    }

    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    // ベクトルを回転させる
    inline XMFLOAT3 RotateVector(XMFLOAT3 euler, XMFLOAT3 v) {
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
            euler.x,
            euler.y,
            euler.z
        );
        XMVECTOR cLocal = XMVectorSet(
            v.x,
            v.y,
            v.z,
            0.0f
        );

        // 回転させる
        XMVECTOR cWorld = XMVector3Transform(cLocal, R);

        XMFLOAT3 rv = {
            XMVectorGetX(cWorld),
            XMVectorGetY(cWorld),
            XMVectorGetZ(cWorld)
        };

        return rv;
    }

    inline XMFLOAT3 RotateVector(XMVECTOR quaternion, XMFLOAT3 v) {
        XMVECTOR q = XMQuaternionNormalize(quaternion);

        XMVECTOR vec = XMLoadFloat3(&v);

        // 回転行列で方向ベクトルを回す（平行移動の影響を受けない）
        XMMATRIX R = XMMatrixRotationQuaternion(q);
        XMVECTOR out = XMVector3TransformNormal(vec, R);

        XMFLOAT3 rv;
        XMStoreFloat3(&rv, out);
        return rv;
    }

}