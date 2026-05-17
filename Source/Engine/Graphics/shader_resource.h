//---------------------------------------------------
// shader_resource.h
// 
// Author：Miu Kitamura
// Date  ：2026/05/14
//---------------------------------------------------
#ifndef SHADER_RESOURCE_H
#define SHADER_RESOURCE_H

#include "Engine/Device/direct3d.h"
using namespace DirectX;

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <string>
#include <array>

#include "shader_definitions.h"
using namespace ShaderDefinitions;

// 頂点シェーダーリソース
class VertexShaderResource {
public:
    std::string filePath;    // シェーダーファイルのパス
    ComPtr<ID3D11VertexShader> vertexShader;  // 頂点シェーダー
    ComPtr<ID3D11InputLayout>  inputLayout;   // 頂点レイアウト
    VertexType vertexType; // 頂点の種類
};

// ピクセルシェーダーリソース
class PixelShaderResource {
public:
    std::string filePath;    // シェーダーファイルのパス
    ComPtr<ID3D11PixelShader>  pixelShader;   // ピクセルシェーダー
};

// 定数バッファの使用方法を表す列挙型
enum class ConstantBufferUsage {
    Dynamic,
    Default,
};

// 定数バッファリソース
class ConstantBufferResource {
public:
    std::string name;   // 定数バッファの名前
    UINT slot;          // シェーダーステージのスロット番号
    UINT byteWidth;     // 定数バッファのサイズ
    bool bindVS;        // 頂点シェーダーにバインドするか
    bool bindPS;        // ピクセルシェーダーにバインドするか
    ComPtr<ID3D11Buffer> buffer; // 定数バッファ
    ConstantBufferUsage usage;  // 定数バッファの使用方法
};

// シェーダープログラムリソース
class ShaderProgramResource {
public:
    std::string name; // シェーダーの名前
    ShaderProgramResource* baseShader = nullptr; // ベースシェーダー

    // == 上書き・追加リソース ==
    VertexShaderResource* overrideVertexShader = nullptr;  // 上書き頂点シェーダー
    PixelShaderResource* overridePixelShader = nullptr;    // 上書きピクセルシェーダー

    std::vector<ConstantBufferResource*> additionalConstantBuffers; // 追加定数バッファ

    // == 最終的に使用するリソース ==
    VertexShaderResource* vertexShader = nullptr;  // 最終頂点シェーダー
    PixelShaderResource* pixelShader = nullptr;    // 最終ピクセルシェーダー

    std::vector<ConstantBufferResource*> constantBuffers;   // 最終定数バッファ

};

#endif // SHADER_RESOURCE_H