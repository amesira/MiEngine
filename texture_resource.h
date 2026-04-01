//---------------------------------------------------
// texture_resource.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/28
//---------------------------------------------------
#ifndef TEXTURE_RESOURCE_H
#define TEXTURE_RESOURCE_H

#include "d3d11.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "direct3d.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <string>

// テクスチャリソース
class TextureResource {
public:
    const wchar_t* filePath; // テクスチャのファイルパス
    ComPtr<ID3D11ShaderResourceView> texture;
};

#endif // TEXTURE_RESOURCE_H