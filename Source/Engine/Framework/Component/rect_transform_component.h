//===================================================
// rect_transform_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
#ifndef RECT_TRANSFORM_COMPONENT_H
#define RECT_TRANSFORM_COMPONENT_H
#include "Engine/Core/component.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

class RectTransformComponent : public Component {
private:
    DirectX::XMFLOAT3   m_position = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3   m_rotation = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3   m_scaling = { 1.0f,1.0f,1.0f };

public:
    void    SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }
    void    SetRotation(DirectX::XMFLOAT3 rotation) { m_rotation = rotation; }
    void    SetScaling(DirectX::XMFLOAT3 scaling) { m_scaling = scaling; }

    DirectX::XMFLOAT3   GetPosition()const { return m_position; }
    DirectX::XMFLOAT3   GetRotation()const { return m_rotation; }
    DirectX::XMFLOAT3   GetScaling()const { return m_scaling; }

};


#endif