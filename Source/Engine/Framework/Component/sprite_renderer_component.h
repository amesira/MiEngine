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
#include "Engine/Graphics/material_resource.h"

class SpriteRendererComponent : public Component {
public:
    enum class SpriteBlendMode
    {
        Opaque,
        Cutout,
        AlphaBlend,
        Additive,
    };

private:
    MaterialInstance m_material;
    XMFLOAT4    m_uvRect = { 0.0f,0.0f,1.0f,1.0f };
    XMFLOAT4    m_color = { 1.0f,1.0f,1.0f,1.0f };

    bool m_flipX = false;
    bool m_flipY = false;

    SpriteBlendMode  m_blendMode = SpriteBlendMode::Opaque;

public:
    // テクスチャリソースの設定・取得
    void    SetTextureResource(TextureResource* resource) {
        m_material.isOverrideAlbedoTexture = true;
        m_material.overrideAlbedoTexture = resource;
    }
    TextureResource* GetTextureResource()const {
        if (m_material.isOverrideAlbedoTexture) return m_material.overrideAlbedoTexture;
        return m_material.materialResource ? m_material.materialResource->albedoTexture : nullptr;
    }

    void SetMaterialResource(MaterialResource* materialResource) { m_material.materialResource = materialResource; }
    MaterialInstance& GetMaterial() { return m_material; }
    const MaterialInstance& GetMaterial() const { return m_material; }
    // UV矩形の設定・取得
    void    SetUvRect(XMFLOAT4 uvRect) { m_uvRect = uvRect; }
    XMFLOAT4   GetUvRect()const { return m_uvRect; }
    // 色の設定・取得
    void    SetColor(XMFLOAT4 color) { m_color = color; }
    XMFLOAT4   GetColor()const { return m_color; }

    // 反転フラグの設定・取得
    void    SetFlipX(bool flipX) { m_flipX = flipX; }
    bool    GetFlipX() const { return m_flipX; }
    void    SetFlipY(bool flipY) { m_flipY = flipY; }
    bool    GetFlipY() const { return m_flipY; }

    // ブレンドモードの設定・取得
    void SetBlendMode(SpriteBlendMode blendMode) { m_blendMode = blendMode; }
    SpriteBlendMode GetBlendMode() const { return m_blendMode; }

};

#endif
