//---------------------------------------------------
// model_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/28
//---------------------------------------------------
#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "component.h"

#include "material_resource.h"
#include "model_resource.h"
#include <iostream>

#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

class ModelComponent : public Component {
private:
    // モデルリソース
    ModelResource*  m_pModelResource = nullptr;
    // マテリアルスロット
    std::vector<MaterialInstance> m_materialSlots;

public:
    // モデルファイルパスの設定・取得
    void    SetModelResource(ModelResource* pModelResource) { 
        m_pModelResource = pModelResource;

        m_materialSlots.clear();
        m_materialSlots.resize(pModelResource->materialResources.size());
        for (size_t i = 0; i < m_materialSlots.size(); i++) {
            m_materialSlots[i].materialResource = pModelResource->materialResources[i];
        }
    }
    ModelResource* GetModelResource() const { return m_pModelResource; }

    // マテリアルスロットの取得
    std::vector<MaterialInstance>& GetMaterialSlots() { return m_materialSlots; }

};

#endif // MODEL_COMPONENT_H