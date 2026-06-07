//---------------------------------------------------
// material_resource.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/28
//---------------------------------------------------
#ifndef MATERIAL_RESOURCE_H
#define MATERIAL_RESOURCE_H
#include "Engine/Device/direct3d.h"
using namespace DirectX;

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <string>

#include "texture_resource.h"
#include "shader_resource.h"

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
    float       emissiveIntensity;

    XMFLOAT2    uvTiling;
    XMFLOAT2    uvOffset;
};

// マテリアルリソース
class MaterialResource {
public:
    std::string name;
    RenderMode  renderMode = RenderMode::Opaque;

    // 使用するシェーダープログラムリソースへのポインタ
    ShaderProgramResource* shaderProgram = nullptr;

    // === PBR基本 ===
    XMFLOAT4    baseColor = { 1,1,1,1 };    // ベースカラー（アルベド）
    float       metallic = 0.0f;            // 金属度
    float       roughness = 1.0f;           // 粗さ

    // === 発光 ===
    XMFLOAT3    emissiveColor = { 0,0,0 };  // 発光色
    float       emissiveIntensity = 1.0f;   // 発光強度

    // === テクスチャ ===
    TextureResource* albedoTexture = nullptr;   // アルベドテクスチャ
    TextureResource* normalTexture = nullptr;   // 法線マップテクスチャ（未実装）
    TextureResource* emissiveTexture = nullptr; // 発光テクスチャ（未実装）
    TextureResource* aoTexture = nullptr;       // アンビエントオクルージョンテクスチャ（未実装）

    // === UV ===
    XMFLOAT2 uvTiling = { 1,1 };    // UVのループ数（未実装）
    XMFLOAT2 uvOffset = { 0,0 };    // UVのオフセット（未実装）

    // === 描画設定 ===
    bool cullBackFace = true;   // 背面カリングするか（未実装）

    // === Custom ===
    static constexpr int CUSTOM_PROPERTY_COUNT = 8;
    XMFLOAT4 customProperties[CUSTOM_PROPERTY_COUNT] = { {0,0,0,0} }; // カスタムプロパティ（シェーダーで自由に使用可能）
    static constexpr int CUSTOM_TEXTURE_COUNT = 4;
    TextureResource* customTextures[CUSTOM_TEXTURE_COUNT] = { nullptr }; // カスタムテクスチャ（シェーダーで自由に使用可能）

    // マテリアルバッファ生成
    MaterialBufferData CreateBufferData() const {
        MaterialBufferData data;
        data.baseColor = baseColor;
        data.metallic = metallic;
        data.roughness = roughness;
        data.emissiveColor = emissiveColor;
        data.emissiveIntensity = emissiveIntensity;
        data.uvTiling = uvTiling;
        data.uvOffset = uvOffset;
        return data;
    }

    // カスタムプロパティの取得
    void GetCustomProperties(XMFLOAT4* outProperties) const {
        for (int i = 0; i < CUSTOM_PROPERTY_COUNT; i++) {
            outProperties[i] = customProperties[i];
        }
    }
    void GetCustomTextures(TextureResource** outTextures) const {
        for (int i = 0; i < CUSTOM_TEXTURE_COUNT; i++) {
            outTextures[i] = customTextures[i];
        }
    }
};

// マテリアルインスタンス
class MaterialInstance {
public:
    // マテリアルリソースへのポインタ
    MaterialResource* materialResource = nullptr;

    // オーバーライド設定
    bool isOverrideAlbedoTexture = false;
    TextureResource* overrideAlbedoTexture = nullptr;
    bool isOverrideBaseColor = false;
    XMFLOAT4 overrideBaseColor = { 1,1,1,1 };
    bool isOverrideEmissive = false;
    XMFLOAT3 overrideEmissiveColor = { 0,0,0 };
    float overrideEmissiveIntensity = 1.0f;

    // カスタムプロパティのオーバーライド設定
    bool isOverrideCustomProperties[MaterialResource::CUSTOM_PROPERTY_COUNT] = { false };
    XMFLOAT4 overrideCustomProperties[MaterialResource::CUSTOM_PROPERTY_COUNT] = { {0,0,0,0} };
};

#endif // MATERIAL_H
