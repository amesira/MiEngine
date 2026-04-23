//===================================================
// image_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
#ifndef IMAGE_COMPONENT_H
#define IMAGE_COMPONENT_H
#include "Engine/Core/component.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

#include "Engine/System/Graphics/texture_resource.h"

enum class WorldSpaceType {
    None,       // 通常表示
    Billboard,  // 常にカメラ方向を向く
    HD2D,       // Y軸回りのみカメラ方向を向く
};

class ImageComponent : public Component {
private:
    TextureResource*   m_pTextureResource = nullptr;
    XMFLOAT4           m_uvRect = { 0.0f,0.0f,1.0f,1.0f };
    XMFLOAT4           m_color = { 1.0f,1.0f,1.0f,1.0f };

    WorldSpaceType  m_worldSpaceType = WorldSpaceType::None;

public:
    void    SetTextureResource(TextureResource* resource) { m_pTextureResource = resource; }
    void    SetUvRect(XMFLOAT4 uvRect) { m_uvRect = uvRect; }
    void    SetColor(XMFLOAT4 color) { m_color = color; }

    TextureResource* GetTextureResource()const { return m_pTextureResource; }
    XMFLOAT4   GetUvRect()const { return m_uvRect; }
    XMFLOAT4   GetColor()const { return m_color; }

   void    SetWorldSpaceType(WorldSpaceType type) { m_worldSpaceType = type; }
   WorldSpaceType  GetWorldSpaceType()const { return m_worldSpaceType; }
};

#endif