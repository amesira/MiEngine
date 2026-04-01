//===================================================
// image_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
#ifndef IMAGE_COMPONENT_H
#define IMAGE_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "component.h"

#include "sprite.h"
#include "engine_service_locator.h"

enum class WorldSpaceType {
    None,       // 通常表示
    Billboard,  // 常にカメラ方向を向く
    HD2D,       // Y軸回りのみカメラ方向を向く
};

class ImageComponent : public Component {
private:
    ID3D11ShaderResourceView*   m_pTexture = nullptr;
    DirectX::XMFLOAT4           m_uvRect = { 0.0f,0.0f,1.0f,1.0f };
    DirectX::XMFLOAT4           m_color = { 1.0f,1.0f,1.0f,1.0f };

    WorldSpaceType  m_worldSpaceType = WorldSpaceType::None;

public:
    void    SetTexture(ID3D11ShaderResourceView* pTexture) { m_pTexture = pTexture; }
    void    SetUvRect(DirectX::XMFLOAT4 uvRect) { m_uvRect = uvRect; }
    void    SetColor(DirectX::XMFLOAT4 color) { m_color = color; }

    ID3D11ShaderResourceView* GetTexture()const { return m_pTexture; }
    DirectX::XMFLOAT4   GetUvRect()const { return m_uvRect; }
    DirectX::XMFLOAT4   GetColor()const { return m_color; }

   void    SetWorldSpaceType(WorldSpaceType type) { m_worldSpaceType = type; }
   WorldSpaceType  GetWorldSpaceType()const { return m_worldSpaceType; }
};

#endif