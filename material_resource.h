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
    Cutout      // 切り抜き（不透明か完全に透明かのどちらか）
};

// マテリアルリソース
class MaterialResource {
public:
    std::string name;
    RenderMode  renderMode = RenderMode::Opaque;

    XMFLOAT4    baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float       metallic = 0.0f;
    float       smoothness = 0.0f;

    // テクスチャリソース
    TextureResource*    albedoTexture = nullptr;
    TextureResource*    normalTexture = nullptr;

};

#endif // MATERIAL_H