#pragma once
#include <DirectXMath.h>
using namespace DirectX;

#include <algorithm>
#include <cmath>
#include <random>

namespace MiMath
{
    inline float Infinity(){
        return std::numeric_limits<float>::infinity();
    }

    // 加算
    inline XMVECTOR Add(const XMVECTOR& va, const XMVECTOR& vb) {
        return XMVectorAdd(va, vb);
    }
    inline XMFLOAT3 Add(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR va = XMLoadFloat3(&a);
        XMVECTOR vb = XMLoadFloat3(&b);
        XMVECTOR vr = Add(va, vb);
        XMFLOAT3 result;
        XMStoreFloat3(&result, vr);
        return result;
    }

    // 減算
    inline XMVECTOR Subtract(const XMVECTOR& va, const XMVECTOR& vb) {
        return XMVectorSubtract(va, vb);
    }
    inline XMFLOAT3 Subtract(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR va = XMLoadFloat3(&a);
        XMVECTOR vb = XMLoadFloat3(&b);
        XMVECTOR vr = Subtract(va, vb);
        XMFLOAT3 result;
        XMStoreFloat3(&result, vr);
        return result;
    }

    // 乗算
    inline XMVECTOR Multiply(const XMVECTOR& va, const XMVECTOR& vb) {
        return XMVectorMultiply(va, vb);
    }
    inline XMFLOAT3 Multiply(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR va = XMLoadFloat3(&a);
        XMVECTOR vb = XMLoadFloat3(&b);
        XMVECTOR vr = Multiply(va, vb);
        XMFLOAT3 result;
        XMStoreFloat3(&result, vr);
        return result;
    }
    inline XMVECTOR Multiply(const XMVECTOR& v, float scalar) {
        return XMVectorScale(v, scalar);
    }
    inline XMFLOAT3 Multiply(const XMFLOAT3& v, float scalar) {
        XMVECTOR vv = XMLoadFloat3(&v);
        XMVECTOR vr = Multiply(vv, scalar);
        XMFLOAT3 result;
        XMStoreFloat3(&result, vr);
        return result;
    }

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
    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    inline XMFLOAT3 Lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t) {
        XMFLOAT3 result = {};
        result.x = Lerp(a.x, b.x, t);
        result.y = Lerp(a.y, b.y, t);
        result.z = Lerp(a.z, b.z, t);
        return result;
    }
    inline XMFLOAT4 Lerp(const XMFLOAT4& a, const XMFLOAT4& b, float t) {
        XMFLOAT4 result = {};
        result.x = Lerp(a.x, b.x, t);
        result.y = Lerp(a.y, b.y, t);
        result.z = Lerp(a.z, b.z, t);
        result.w = Lerp(a.w, b.w, t);
        return result;
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
    inline XMFLOAT3 RotateVector(XMFLOAT4 quaternion, XMFLOAT3 v) {
        return RotateVector(XMLoadFloat4(&quaternion), v);
    }

    // Pitch, Yaw, Rollからクォータニオンを作成する
    inline XMFLOAT4 QuaternionFromEuler(XMFLOAT3 euler) {
        XMFLOAT4 q;
        XMStoreFloat4(&q, XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z));
        return q;
    }

    inline XMFLOAT4 Slerp(const XMFLOAT4& current, const XMFLOAT4& target, float t) {
        t = Clamp(t, 0.0f, 1.0f);

        XMVECTOR currentQuaternion = XMQuaternionNormalize(XMLoadFloat4(&current));
        XMVECTOR targetQuaternion = XMQuaternionNormalize(XMLoadFloat4(&target));

        XMFLOAT4 result;
        XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionSlerp(currentQuaternion, targetQuaternion, t)));
        return result;
    }

    // SmoothDamp関数の実装
    inline XMFLOAT3 SmoothDamp(const XMFLOAT3& current, const XMFLOAT3& target, XMFLOAT3& currentVelocity, float smoothTime, float deltaTime)
    {
        if (smoothTime < 1e-4f) {
            smoothTime = 1e-4f;
        }
        float omega = 2.0f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        XMFLOAT3 change = Subtract(current, target);
        XMFLOAT3 temp = Add(currentVelocity, Multiply(change, omega));
        temp = Multiply(temp, deltaTime);

        XMFLOAT3 result = Multiply(Add(change, temp), exp);
        result = Add(target, result);

        currentVelocity = Multiply(Subtract(currentVelocity, Multiply(temp, omega)), exp);
        return result;
    }
    inline float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float deltaTime)
    {
        if (smoothTime < 1e-4f) {
            smoothTime = 1e-4f;
        }
        float omega = 2.0f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        float change = current - target;
        float temp = (currentVelocity + omega * change) * deltaTime;

        float result = target + (change + temp) * exp;

        currentVelocity = (currentVelocity - temp * omega) * exp;
        return result;
    }

    // === ランダム関連の関数 ===
    // 乱数生成器の取得
    inline std::mt19937& GetRandomEngine()
    {
        static std::mt19937 engine{ std::random_device{}() };
        return engine;
    }
    // minValue以上maxValue未満の範囲でランダムなfloat値を生成する
    inline float RandomRange(float minValue, float maxValue)
    {
        std::uniform_real_distribution<float> dist(minValue, maxValue);
        return dist(GetRandomEngine());
    }
    // ランダムな単位ベクトルを生成する
    inline XMFLOAT3 RandomUnitVector()
    {
        const float z = RandomRange(-1.0f, 1.0f);
        const float angle = RandomRange(0.0f, XM_2PI);
        const float radius = std::sqrt((1.0f - z * z) > 0.0f ? (1.0f - z * z) : 0.0f);
        return {
            radius * std::cos(angle),
            radius * std::sin(angle),
            z
        };
    }
}
