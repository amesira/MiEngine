//===================================================
// shader_manager.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/01
//===================================================
#include "shader_manager.h"

#include "Utility/debug_ostream.h"
#include "Engine/engine_service_locator.h"

#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

// シェーダー管理の初期化
bool ShaderManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // TransformBufferとCameraBuffer用の定数バッファを作成
    m_transformCB = SHADER_REPOSITORY->GenerateConstantBufferResource(
        "TransformBuffer", 
        0,
        sizeof(TransformBuffer), 
        true,
        true,
        ConstantBufferUsage::Dynamic);
    if (!m_transformCB) {
        hal::dout << "ShaderManager::Initialize() : TransformBuffer用定数バッファの作成に失敗しました" << std::endl;
        return false;
    }

    m_cameraCB = SHADER_REPOSITORY->GenerateConstantBufferResource(
        "CameraBuffer",
        1,
        sizeof(CameraBuffer),
        true,
        true,
        ConstantBufferUsage::Dynamic);
    if (!m_cameraCB) {
        hal::dout << "ShaderManager::Initialize() : CameraBuffer用定数バッファの作成に失敗しました" << std::endl;
        return false;
    }

    // 定数バッファをベースシェーダーに登録
    for (size_t index = 0; index < static_cast<size_t>(ShaderBase::MAX); index++) {
        SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[index], m_transformCB);
        SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[index], m_cameraCB);
    }

    return true;
}

// シェーダー管理の終了処理
void ShaderManager::Finalize()
{
    
}

// シェーダーをバインドする関数
void ShaderManager::BindShader(ShaderBase shaderBase)
{
    const std::string shaderBaseName = SHADER_BASE_NAMES[static_cast<size_t>(shaderBase)];
    ShaderProgramResource* shaderProgram = SHADER_REPOSITORY->GetShaderProgramResource(shaderBaseName);
    BindShader(shaderProgram);
}
void ShaderManager::BindShader(const std::string& shaderName)
{
    ShaderProgramResource* shaderProgram = SHADER_REPOSITORY->GetShaderProgramResource(shaderName);
    BindShader(shaderProgram);
}
void ShaderManager::BindShader(const ShaderProgramResource* shaderProgram)
{
    if (shaderProgram) {
        m_pContext->VSSetShader(shaderProgram->vertexShader->vertexShader.Get(), nullptr, 0);
        m_pContext->PSSetShader(shaderProgram->pixelShader->pixelShader.Get(), nullptr, 0);
        m_pContext->IASetInputLayout(shaderProgram->vertexShader->inputLayout.Get());
        for (const auto& cbResource : shaderProgram->constantBuffers) {
            if (cbResource) {
                if (cbResource->bindVS) {
                    m_pContext->VSSetConstantBuffers(cbResource->slot, 1, cbResource->buffer.GetAddressOf());
                }
                if (cbResource->bindPS) {
                    m_pContext->PSSetConstantBuffers(cbResource->slot, 1, cbResource->buffer.GetAddressOf());
                }
            }
        }
    }
}

// -------------------------- Bind Buffer

// TransformBufferを更新する関数
void ShaderManager::BindTransformCB(const TransformBuffer& transformData)
{
    // TransPose変換
    XMMATRIX transposedWorld = XMMatrixTranspose(transformData.world);
    XMMATRIX transposedNormal = XMMatrixTranspose(transformData.normal);

    // データをGPUに転送
    D3D11_MAPPED_SUBRESOURCE msr = {};
    m_pContext->Map(m_transformCB->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    {
        TransformBuffer* tb = (TransformBuffer*)msr.pData;
        tb->world = transposedWorld;
        tb->normal = transposedNormal;
        tb->invWorld = XMMatrixInverse(nullptr, transposedWorld);
    }
    m_pContext->Unmap(m_transformCB->buffer.Get(), 0);

    /*TransformBuffer data = { transposedWorld, transposedNormal, XMMatrixInverse(nullptr, transposedWorld) };
    m_pContext->UpdateSubresource(m_transformCB->buffer.Get(), 0, nullptr, &data, 0, 0);*/
}
// CameraBufferを更新する関数
void ShaderManager::BindCameraCB(const CameraBuffer& cameraData)
{
    // TransPose変換
    XMMATRIX transposedView = XMMatrixTranspose(cameraData.view);
    XMMATRIX transposedProjection = XMMatrixTranspose(cameraData.projection);

    // データをGPUに転送
    D3D11_MAPPED_SUBRESOURCE msr = {};
    m_pContext->Map(m_cameraCB->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    {
        CameraBuffer* cb = (CameraBuffer*)msr.pData;
        cb->view = transposedView;
        cb->projection = transposedProjection;
        cb->eyePos = cameraData.eyePos;
        cb->invView = XMMatrixInverse(nullptr, transposedView);
        cb->invProjection = XMMatrixInverse(nullptr, transposedProjection);
    }
    m_pContext->Unmap(m_cameraCB->buffer.Get(), 0);

    /*CameraBuffer data = { 
        transposedView, 
        transposedProjection, 
        cameraData.eyePos, 
        XMMatrixInverse(nullptr, transposedView), 
        XMMatrixInverse(nullptr, transposedProjection) };
    m_pContext->UpdateSubresource(m_cameraCB->buffer.Get(), 0, nullptr, &data, 0, 0);*/
}

//----------------------------------------------------------------------------- private
//
//// VertexShaderの読み込み
//bool ShaderManager::LoadVertexShader(const char* filename, ID3D11VertexShader** ppVertexShader, VsBinaryData& vbData)
//{
//    std::ifstream ifs_vs(filename, std::ios::binary);
//    if (!ifs_vs)return false;
//
//    // ファイルサイズを取得
//    ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
//    vbData.fileSize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
//    ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す
//
//    // バイナリデータを格納するためのバッファを確保
//    vbData.vsBinaryPointer = new unsigned char[vbData.fileSize];
//
//    ifs_vs.read((char*)vbData.vsBinaryPointer, vbData.fileSize); // バイナリデータを読み込む
//    ifs_vs.close(); // ファイルを閉じる
//
//    // 頂点シェーダーの作成
//    HRESULT hr = m_pDevice->CreateVertexShader(vbData.vsBinaryPointer, vbData.fileSize, nullptr, ppVertexShader);
//    if (FAILED(hr)) {
//        delete[] vbData.vsBinaryPointer; // メモリリークしないようにバイナリデータのバッファを解放
//        return false;
//    }
//
//    return true;
//}
//
//// PixelShaderの読み込み
//bool ShaderManager::LoadPixelShader(const char* filename, ID3D11PixelShader** ppPixelShader)
//{
//    // 事前コンパイル済みピクセルシェーダーの読み込み
//    std::ifstream ifs_ps(filename, std::ios::binary);
//    if (!ifs_ps)return false;
//
//    // ファイルサイズを取得
//    ifs_ps.seekg(0, std::ios::end);
//    std::streamsize filesize = ifs_ps.tellg();
//    ifs_ps.seekg(0, std::ios::beg);
//
//    // バイナリデータを格納するためのバッファを確保
//    unsigned char* psbinary_pointer = new unsigned char[filesize];
//    ifs_ps.read((char*)psbinary_pointer, filesize);
//    ifs_ps.close();
//
//    // ピクセルシェーダーの作成
//    HRESULT hr = m_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, ppPixelShader);
//
//    delete[] psbinary_pointer; // バイナリデータのバッファを解放
//
//    if (FAILED(hr))return false;
//
//    return true;
//}
