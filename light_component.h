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
private:
    XMFLOAT4 m_direction = { 0.0f, -1.0f, 0.0f, 0.0f }; // 方向（w=0.0fで方向光源）
    XMFLOAT4 m_diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };    // 拡散光
    XMFLOAT4 m_ambient = { 0.2f, 0.2f, 0.2f, 1.0f };    // 環境光

public:
    void SetDirection(const XMFLOAT4& direction) { m_direction = direction; }
    void SetDiffuse(const XMFLOAT4& diffuse) { m_diffuse = diffuse; }
    void SetAmbient(const XMFLOAT4& ambient) { m_ambient = ambient; }

    XMFLOAT4 GetDirection() const { return m_direction; }
    XMFLOAT4 GetDiffuse() const { return m_diffuse; }
    XMFLOAT4 GetAmbient() const { return m_ambient; }

};

#endif // LIGHT_COMPONENT_H