//---------------------------------------------------
// light_component.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/03
//---------------------------------------------------
#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "component.h"
#include <DirectXMath.h>
using namespace DirectX;

class LightComponent : public Component {
public:
    enum class LightType {
        Directional,
        Point,
        Spot
    };

private:
    LightType m_lightType = LightType::Directional; // ライトの種類

    XMFLOAT4 m_direction = { 0.0f, -1.0f, 0.0f, 0.0f }; // 方向（w=0.0fで方向光源）
    XMFLOAT4 m_diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };    // 拡散光
    XMFLOAT4 m_ambient = { 0.2f, 0.2f, 0.2f, 1.0f };    // 環境光

    float m_intensity = 1.0f;   // 光の強さ

    float m_range = 10.0f;      // 範囲（点光源・スポットライト用）
    float m_spotAngle = 30.0f;  // スポットライトの角度（スポットライト用）

public:
    // ライトの種類設定・取得
    void SetLightType(LightType type) { m_lightType = type; }
    LightType GetLightType() const { return m_lightType; }

    // ライトのパラメータ設定・取得
    void SetDirection(const XMFLOAT4& direction) { m_direction = direction; }
    void SetDiffuse(const XMFLOAT4& diffuse) { m_diffuse = diffuse; }
    void SetAmbient(const XMFLOAT4& ambient) { m_ambient = ambient; }
    XMFLOAT4 GetDirection() const { return m_direction; }
    XMFLOAT4 GetDiffuse() const { return m_diffuse; }
    XMFLOAT4 GetAmbient() const { return m_ambient; }

    // 光の強さ設定・取得
    void SetIntensity(float intensity) { m_intensity = intensity; }
    float GetIntensity() const { return m_intensity; }

    // 点光源・スポットライト用のパラメータ設定・取得
    void SetRange(float range) { m_range = range; }
    void SetSpotAngle(float angle) { m_spotAngle = angle; }
    float GetRange() const { return m_range; }
    float GetSpotAngle() const { return m_spotAngle; }

};

#endif // LIGHT_COMPONENT_H