//---------------------------------------------------
// shader_manager.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/01
//---------------------------------------------------
#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H
#include "direct3d.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <array>
#include <fstream>
#include <wrl/client.h>
using namespace Microsoft::WRL;

// 頂点構造体
struct LitVertex {
    XMFLOAT3 position;  // 頂点の位置
    XMFLOAT3 normal;    // 頂点の法線
    XMFLOAT4 color;     // 頂点の色
    XMFLOAT2 texCoord;  // 頂点のテクスチャ座標
};
struct SkinnedLitVertex {
    XMFLOAT3 position;  // 頂点の位置
    XMFLOAT3 normal;    // 頂点の法線
    XMFLOAT4 color;     // 頂点の色
    XMFLOAT2 texCoord;  // 頂点のテクスチャ座標

    XMUINT4   boneIndices; // ボーンのインデックス
    XMFLOAT4 boneWeights; // ボーンの重み
};

// シェーダー管理クラス
class ShaderManager {
public:
    // シェーダーの種類
    enum class ShaderType {
        Lit,
        SkinnedLit,
        DecalLit,

        Unlit,
        TlueTypeFontUnlit,

        MAX,
    };
    // シェーダーコンテナ構造体
    struct ShaderContainer {
        ComPtr<ID3D11VertexShader> vertexShader;  // 頂点シェーダー
        ComPtr<ID3D11PixelShader>  pixelShader;   // ピクセルシェーダー

        ComPtr<ID3D11InputLayout>  inputLayout;   // 頂点レイアウト
        std::array<ID3D11Buffer**, 15> constantBuffers; // 定数バッファの配列（b0~b14まで対応）
    };

    // TransformBuffer構造体
    struct TransformBuffer {
        XMMATRIX world; // ワールド行列
        XMMATRIX normal;// 法線行列
    };
    // CameraBuffer構造体
    struct CameraBuffer {
        XMMATRIX view;       // ビュー行列
        XMMATRIX projection; // プロジェクション行列
        XMFLOAT4 eyePos;     // カメラの位置
    };

private:
    // 頂点バイナリデータ構造体
    struct VsBinaryData {
        unsigned char* vsBinaryPointer; // バイナリデータへのポインタ
        std::streamsize	fileSize;        // バイナリデータのサイズ
    };
    // Direct3Dデバイスとデバイスコンテキストへのポインタ
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;

    // 現在バインドされているシェーダーの種類
    ShaderType m_shaderType;
    // シェーダーコンテナの配列
    ShaderContainer m_shaderContainer[static_cast<size_t>(ShaderType::MAX)];

    // TransformBuffer
    ID3D11Buffer* m_transformCB;
    // CameraBuffer
    ID3D11Buffer* m_cameraCB;

public:
    ShaderManager() = default;

    // シェーダー管理の初期化
    bool    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    // シェーダー管理の終了処理
    void    Finalize();

    // 定数バッファをシェーダーコンテナに登録する関数
    void    RegisterCB(ShaderType shaderType, UINT slot, ID3D11Buffer** ppBuffer);
    // シェーダーをバインドする関数
    void    BindShader(ShaderType shaderType);

    // TransformBufferをバインドする関数
    void    BindTransformCB(const TransformBuffer& transformData);
    // CameraBufferをバインドする関数
    void    BindCameraCB(const CameraBuffer& cameraData);

private:
    // 頂点シェーダー読み込み
    bool    LoadVertexShader(const char* filename, ID3D11VertexShader** ppVertexShader, VsBinaryData& vbData);
    // ピクセルシェーダー読み込み
    bool    LoadPixelShader(const char* filename, ID3D11PixelShader** ppPixelShader);

};

#endif // SHADER_MANAGER_H