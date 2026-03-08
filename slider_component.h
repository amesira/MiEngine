//----------------------------------------------------
// slider_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/26
//----------------------------------------------------
#ifndef SLIDER_COMPONENT_H
#define SLIDER_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "component.h"

class SliderComponent : public Component {
private:
    float               m_value = 0.0f; // 0.0f～1.0fの範囲
    DirectX::XMFLOAT4   m_bgColor = { 0.2f,0.2f,0.2f,1.0f };    // 背景色
    DirectX::XMFLOAT4   m_fillColor = { 0.8f,0.8f,0.8f,1.0f };  // 塗りつぶし色

public:
    void    SetValue(float value) {
        m_value = value;
        if (m_value < 0.0f) m_value = 0.0f;
        else if (m_value > 1.0f) m_value = 1.0f;
    }
    void    SetBgColor(DirectX::XMFLOAT4 color) { m_bgColor = color; }
    void    SetFillColor(DirectX::XMFLOAT4 color) { m_fillColor = color; }

    float               GetValue() const { return m_value; }
    DirectX::XMFLOAT4   GetBgColor() const { return m_bgColor; }
    DirectX::XMFLOAT4   GetFillColor() const { return m_fillColor; }

};

#endif