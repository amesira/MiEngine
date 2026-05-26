//===================================================
// sprite_renderer_component.h
// 
// Author：Miu Kitamura
// Date  ：2026/05/26
//===================================================
#ifndef SPRITE_RENDERER_COMPONENT_H
#define SPRITE_RENDERER_COMPONENT_H
#include "Engine/Core/component.h"

#include "Engine/Device/direct3d.h"
using namespace DirectX;

#include "Engine/Graphics/texture_resource.h"

class SpriteRendererComponent : public Component {
public:
    enum class WorldSpaceType {
        None,       // 通常表示
        Billboard,  // 常にカメラ方向を向く
        HD2D,       // Y軸回りのみカメラ方向を向く
    };

    enum class SpriteBlendMode
    {
        Opaque,
        Cutout,
        AlphaBlend,
        Additive,
    };

private:
    TextureResource*    m_pTextureResource = nullptr;
    XMFLOAT4    m_uvRect = { 0.0f,0.0f,1.0f,1.0f };
    XMFLOAT4    m_color = { 1.0f,1.0f,1.0f,1.0f };

    WorldSpaceType  m_worldSpaceType = WorldSpaceType::None;
    SpriteBlendMode  m_blendMode = SpriteBlendMode::Opaque;

public:
    // テクスチャリソースの設定・取得
    void    SetTextureResource(TextureResource* resource) { m_pTextureResource = resource; }
    TextureResource* GetTextureResource()const { return m_pTextureResource; }
    // UV矩形の設定・取得
    void    SetUvRect(XMFLOAT4 uvRect) { m_uvRect = uvRect; }
    XMFLOAT4   GetUvRect()const { return m_uvRect; }
    // 色の設定・取得
    void    SetColor(XMFLOAT4 color) { m_color = color; }
    XMFLOAT4   GetColor()const { return m_color; }

    // ワールドスペースタイプの設定・取得
    void    SetWorldSpaceType(WorldSpaceType type) { m_worldSpaceType = type; }
    WorldSpaceType  GetWorldSpaceType()const { return m_worldSpaceType; }

    // ブレンドモードの設定・取得
    void SetBlendMode(SpriteBlendMode blendMode) { m_blendMode = blendMode; }
    SpriteBlendMode GetBlendMode() const { return m_blendMode; }

};

#endif