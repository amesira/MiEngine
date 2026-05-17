//---------------------------------------------------
// shader_manager.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/01
//---------------------------------------------------
#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H
#include "Engine/Device/direct3d.h"
using namespace DirectX;

#include <array>
#include <fstream>

#include <wrl/client.h>
using namespace Microsoft::WRL;

#include "Engine/Graphics/shader_resource.h"
#include "Engine/Graphics/shader_definitions.h"

// シェーダー管理クラス
class ShaderManager {
public:
    // TransformBuffer構造体
    struct TransformBuffer {
        XMMATRIX world; // ワールド行列
        XMMATRIX normal;// 法線行列

        XMMATRIX invWorld; // ワールド行列の逆行列
    };
    // CameraBuffer構造体
    struct CameraBuffer {
        XMMATRIX view;       // ビュー行列
        XMMATRIX projection; // プロジェクション行列
        XMFLOAT4 eyePos;     // カメラの位置

        XMMATRIX invView; // ビュー行列の逆行列
        XMMATRIX invProjection; // プロジェクション行列の逆行列
    };

private:
    
    // Direct3Dデバイスとデバイスコンテキストへのポインタ
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;

    // Transform定数バッファリソース
    ConstantBufferResource* m_transformCB;
    // Camera定数バッファリソース
    ConstantBufferResource* m_cameraCB;

public:
    ShaderManager() = default;

    // シェーダー管理の初期化
    bool    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    // シェーダー管理の終了処理
    void    Finalize();

    // シェーダーをバインドする関数
    void    BindShader(ShaderBase shaderBase);
    void    BindShader(const std::string& shaderName);
    void    BindShader(const ShaderProgramResource* shaderProgram);

    // TransformBufferをバインドする関数
    void    BindTransformCB(const TransformBuffer& transformData);
    // CameraBufferをバインドする関数
    void    BindCameraCB(const CameraBuffer& cameraData);

};

#endif // SHADER_MANAGER_H