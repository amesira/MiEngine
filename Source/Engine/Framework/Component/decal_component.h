//---------------------------------------------------
// decal_component.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/13
//---------------------------------------------------
#ifndef DECAL_COMPONENT_H
#define DECAL_COMPONENT_H
#include "Engine/Core/component.h"

#include "Engine/System/Graphics/texture_resource.h"
#include "Engine/System/Graphics/material_resource.h"

#include <DirectXMath.h>
using namespace DirectX;

// デカールコンポーネント
class DecalComponent :public Component {
private:
    // デカール用テクスチャ
    TextureResource* m_decalTexture = nullptr;

    // 投影用パラメータ
    XMFLOAT3 m_projDirection = { 0.0f, -1.0f, 0.0f };  // 投影方向（Transformの回転を考慮しない方向）
    XMFLOAT2 m_projSize = { 1.0f, 1.0f };              // 投影サイズ
    float   m_projDepth = 3.0f;                        // 投影深度

public:
    // デカールテクスチャの設定・取得
    void SetDecalTexture(TextureResource* texture) { m_decalTexture = texture; }
    TextureResource* GetDecalTexture() const { return m_decalTexture; }

    // 投影方向の設定・取得
    void SetProjectionDirection(const XMFLOAT3& direction) { m_projDirection = direction; }
    XMFLOAT3 GetProjectionDirection() const { return m_projDirection; }
    // 投影サイズの設定・取得
    void SetProjectionSize(const XMFLOAT2& size) { m_projSize = size; }
    XMFLOAT2 GetProjectionSize() const { return m_projSize; }
    // 投影深度の設定・取得
    void SetProjectionDepth(float depth) { m_projDepth = depth; }
    float GetProjectionDepth() const { return m_projDepth; }

};

#endif // !DECAL_COMPONENT_H