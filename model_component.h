//---------------------------------------------------
// model_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/28
//---------------------------------------------------
#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "component.h"

#include "model.h"
#include <iostream>

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

class ModelComponent : public Component {
private:
    std::unique_ptr<MODEL>  m_model = nullptr;
    DirectX::XMFLOAT4       m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
    void    SetModel(std::unique_ptr<MODEL> model) { m_model = std::move(model); }
    MODEL*  GetModel() const { return m_model.get(); }

    void    SetColor(DirectX::XMFLOAT4 color) { m_color = color; }
    DirectX::XMFLOAT4   GetColor() const { return m_color; }

    // モデル読み込み
    void LoadModel(const char* filePath) {
        m_model = std::unique_ptr<MODEL>(ModelLoad(filePath));
    }
};

#endif // MODEL_COMPONENT_H