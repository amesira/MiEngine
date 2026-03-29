//---------------------------------------------------
// model_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/28
//---------------------------------------------------
#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "component.h"

#include "model_resource.h"
#include <iostream>

#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

class ModelComponent : public Component {
private:
    ModelResource*  m_pModelResource = nullptr;
    XMFLOAT4        m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
    // モデルファイルパスの設定・取得
    void    SetModelResource(ModelResource* pModelResource) { m_pModelResource = pModelResource; }
    ModelResource* GetModelResource() const { return m_pModelResource; }

    // 色の設定・取得
    void    SetColor(XMFLOAT4 color) { m_color = color; }
    XMFLOAT4   GetColor() const { return m_color; }

};

#endif // MODEL_COMPONENT_H