//---------------------------------------------------
// material_resource.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/28
//---------------------------------------------------
#ifndef MATERIAL_RESOURCE_H
#define MATERIAL_RESOURCE_H
#include "d3d11.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "direct3d.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <string>

#include "texture_resource.h"

// レンダリングモード
enum class RenderMode {
    Opaque,     // 不透明
    Transparent,// 透明
    Cutout      // 切り抜き（不透明か完全に透明かのどちらか。0.5fを閾値とする）
};

// マテリアルバッファ用構造体
struct MaterialBufferData {
    XMFLOAT4    baseColor;
    float       metallic;
    float       roughness;
    float padding[2];

    XMFLOAT3    emissiveColor;
    float padding2;

    XMFLOAT2    uvTiling;
    XMFLOAT2    uvOffset;
};

// マテリアルリソース
class MaterialResource {
public:
    std::string name;
    RenderMode  renderMode = RenderMode::Opaque;

    // === PBR基本 ===
    XMFLOAT4    baseColor = { 1,1,1,1 };    // ベースカラー（アルベド）
    float       metallic = 0.0f;            // 金属度
    float       roughness = 1.0f;           // 粗さ

    // === 発光 ===
    XMFLOAT3    emissiveColor = { 0,0,0 };  // 発光色

    // === テクスチャ ===
    TextureResource* albedoTexture = nullptr;   // アルベドテクスチャ
    TextureResource* normalTexture = nullptr;   // 法線マップテクスチャ
    TextureResource* emissiveTexture = nullptr; // 発光テクスチャ
    TextureResource* aoTexture = nullptr;       // アンビエントオクルージョンテクスチャ

    // === UV ===
    XMFLOAT2 uvTiling = { 1,1 };    // UVのループ数
    XMFLOAT2 uvOffset = { 0,0 };    // UVのオフセット

    // === 描画設定 ===
    bool cullBackFace = true;   // 背面カリングするか

    // マテリアルバッファ生成
    MaterialBufferData CreateBufferData() const {
        MaterialBufferData data;
        data.baseColor = baseColor;
        data.metallic = metallic;
        data.roughness = roughness;
        data.emissiveColor = emissiveColor;
        data.uvTiling = uvTiling;
        data.uvOffset = uvOffset;
        return data;
    }
};

// マテリアルインスタンス
class MaterialInstance {
public:
    MaterialResource* materialResource = nullptr;

    bool isOverrideBaseColor = false;
    XMFLOAT4 overrideBaseColor = { 1,1,1,1 };
    bool isOverrideEmissiveColor = false;
    XMFLOAT3 overrideEmissiveColor = { 0,0,0 };
};

#endif // MATERIAL_H