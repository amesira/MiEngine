//===================================================
// shader_manager.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/01
//===================================================
#include "shader_manager.h"

#include "debug_ostream.h"

// シェーダー管理の初期化
bool ShaderManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    //----------------------------------------------------
    // Shaderの読み込み
    //----------------------------------------------------
#pragma region LoadShader
    size_t i = -1;
    VsBinaryData vbData{};
    HRESULT hr;

    // LitShaderの読み込み
    {
        i = static_cast<size_t>(ShaderType::Lit);

        if (!LoadVertexShader("lit_vs.cso", &m_shaderContainer[i].vertexShader, vbData)) {
            hal::dout << "ShaderManager::Initialize() : LitShaderの頂点シェーダーの作成に失敗しました" << std::endl;
            return false;
        }
        if (!LoadPixelShader("lit_ps.cso", &m_shaderContainer[i].pixelShader)) {
            hal::dout << "ShaderManager::Initialize() : LitShaderのピクセルシェーダーの作成に失敗しました" << std::endl;
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得
        hr = m_pDevice->CreateInputLayout(layout, num_elements, vbData.vsBinaryPointer, vbData.fileSize, &m_shaderContainer[i].inputLayout);
        delete[] vbData.vsBinaryPointer;
        if (FAILED(hr)) {
            hal::dout << "ShaderManager::Initialize() : LitShaderの頂点レイアウトの作成に失敗しました" << std::endl;
            return false;
        }
    }

    // SkinnedLitShaderの読み込み
    {
        i = static_cast<size_t>(ShaderType::SkinnedLit);

        if (!LoadVertexShader("skinned_lit_vs.cso", &m_shaderContainer[i].vertexShader, vbData)) {
            hal::dout << "ShaderManager::Initialize() : SkinnedLitShaderの頂点シェーダーの作成に失敗しました" << std::endl;
            return false;
        }
        if (!LoadPixelShader("lit_ps.cso", &m_shaderContainer[i].pixelShader)) {
            hal::dout << "ShaderManager::Initialize() : SkinnedLitShaderのピクセルシェーダーの作成に失敗しました" << std::endl;
            return false;
        }
        
        D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得
        hr = m_pDevice->CreateInputLayout(layout, num_elements, vbData.vsBinaryPointer, vbData.fileSize, &m_shaderContainer[i].inputLayout);
        delete[] vbData.vsBinaryPointer;
        if (FAILED(hr)) {
            hal::dout << "ShaderManager::Initialize() : SkinnedLitShaderの頂点レイアウトの作成に失敗しました" << std::endl;
            return false;
        }
    }

    // UnlitShaderの読み込み
    {
        i = static_cast<size_t>(ShaderType::Unlit);

        if (!LoadVertexShader("unlit_vs.cso", &m_shaderContainer[i].vertexShader, vbData)) {
            hal::dout << "ShaderManager::Initialize() : UnlitShaderの頂点シェーダーの作成に失敗しました" << std::endl;
            return false;
        }
        if (!LoadPixelShader("unlit_ps.cso", &m_shaderContainer[i].pixelShader)) {
            hal::dout << "ShaderManager::Initialize() : UnlitShaderのピクセルシェーダーの作成に失敗しました" << std::endl;
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得
        hr = m_pDevice->CreateInputLayout(layout, num_elements, vbData.vsBinaryPointer, vbData.fileSize, &m_shaderContainer[i].inputLayout);
        delete[] vbData.vsBinaryPointer;
        if (FAILED(hr)) {
            hal::dout << "ShaderManager::Initialize() : UnlitShaderの頂点レイアウトの作成に失敗しました" << std::endl;
            return false;
        }
    }

    // TrueTypeFontUnlitShaderの読み込み
    {
        i = static_cast<size_t>(ShaderType::TlueTypeFontUnlit);

        /*if (!LoadVertexShader("ttf_unlit_vertex.cso", &m_shaderContainer[i].vertexShader, vbData)) {
            hal::dout << "ShaderManager::Initialize() : TrueTypeFontUnlitShaderの頂点シェーダーの作成に失敗しました" << std::endl;
            return false;
        }
        if (!LoadPixelShader("ttf_unlit_pixel.cso", &m_shaderContainer[i].pixelShader)) {
            hal::dout << "ShaderManager::Initialize() : TrueTypeFontUnlitShaderのピクセルシェーダーの作成に失敗しました" << std::endl;
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };*/

        if (!LoadVertexShader("shader_vertex.cso", &m_shaderContainer[i].vertexShader, vbData)) {
            hal::dout << "ShaderManager::Initialize() : TrueTypeFontUnlitShaderの頂点シェーダーの作成に失敗しました" << std::endl;
            return false;
        }
        if (!LoadPixelShader("shader_pixel_font.cso", &m_shaderContainer[i].pixelShader)) {
            hal::dout << "ShaderManager::Initialize() : TrueTypeFontUnlitShaderのピクセルシェーダーの作成に失敗しました" << std::endl;
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

            { "INS_WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "INS_WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "INS_WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "INS_WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "INS_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "INS_UVRECT",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };

        UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得
        hr = m_pDevice->CreateInputLayout(layout, num_elements, vbData.vsBinaryPointer, vbData.fileSize, &m_shaderContainer[i].inputLayout);
        delete[] vbData.vsBinaryPointer;
        if (FAILED(hr)) {
            hal::dout << "ShaderManager::Initialize() : TrueTypeFontUnlitShaderの頂点レイアウトの作成に失敗しました" << std::endl;
            return false;
        }
    }

    // DefaultShaderの読み込み（旧シェーダーを仮置きしておく）
    {
        i = static_cast<size_t>(ShaderType::Default);

        if (!LoadVertexShader("shader_vertex.cso", &m_shaderContainer[i].vertexShader, vbData)) {
            hal::dout << "ShaderManager::Initialize() : DefaultShaderの頂点シェーダーの作成に失敗しました" << std::endl;
            return false;
        }
        if (!LoadPixelShader("shader_pixel.cso", &m_shaderContainer[i].pixelShader)) {
            hal::dout << "ShaderManager::Initialize() : DefaultShaderのピクセルシェーダーの作成に失敗しました" << std::endl;
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC layout[] = {
             { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
             { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
             { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
             { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

             { "INS_WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
             { "INS_WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
             { "INS_WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
             { "INS_WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
             { "INS_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
             { "INS_UVRECT",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };
        UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得
        hr = m_pDevice->CreateInputLayout(layout, num_elements, vbData.vsBinaryPointer, vbData.fileSize, &m_shaderContainer[i].inputLayout);
        delete[] vbData.vsBinaryPointer;
        if (FAILED(hr)) {
            hal::dout << "ShaderManager::Initialize() : DefaultShaderの頂点レイアウトの作成に失敗しました" << std::endl;
            return false;
        }
    }

#pragma endregion

    //----------------------------------------------------
	// 定数バッファの作成と登録
	//----------------------------------------------------
#pragma region CreateConstantBuffer
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    // TransformBuffer用の定数バッファの作成
    {
        buffer_desc.ByteWidth = sizeof(TransformBuffer);
        hr = m_pDevice->CreateBuffer(&buffer_desc, nullptr, &m_transformCB);
        if (FAILED(hr)) {
            hal::dout << "ShaderManager::Initialize() : TransformBuffer用定数バッファの作成に失敗しました" << std::endl;
            return false;
        }
    }

    // CameraBuffer用の定数バッファの作成
    {
        buffer_desc.ByteWidth = sizeof(CameraBuffer);
        hr = m_pDevice->CreateBuffer(&buffer_desc, nullptr, &m_cameraCB);
        if (FAILED(hr)) {
            hal::dout << "ShaderManager::Initialize() : CameraBuffer用定数バッファの作成に失敗しました" << std::endl;
            return false;
        }
    }
    
    // 定数バッファをシェーダーコンテナに登録
    for (size_t index = 0; index < static_cast<size_t>(ShaderType::MAX); index++) {
        for (UINT slot = 0; slot < 15; slot++) {
            m_shaderContainer[index].constantBuffers[slot] = nullptr;
        }
    }
    RegisterCB(ShaderType::Default, 0, &m_transformCB);
    RegisterCB(ShaderType::Default, 1, &m_cameraCB);

    RegisterCB(ShaderType::Lit, 0, &m_transformCB);
    RegisterCB(ShaderType::Lit, 1, &m_cameraCB);

    RegisterCB(ShaderType::SkinnedLit, 0, &m_transformCB);
    RegisterCB(ShaderType::SkinnedLit, 1, &m_cameraCB);

    RegisterCB(ShaderType::Unlit, 0, &m_transformCB);
    RegisterCB(ShaderType::Unlit, 1, &m_cameraCB);

}

// シェーダー管理の終了処理
void ShaderManager::Finalize()
{
    // シェーダーの解放
    for (size_t i = 0; i < static_cast<size_t>(ShaderType::MAX); ++i) {
        SAFE_RELEASE(m_shaderContainer[i].vertexShader);
        SAFE_RELEASE(m_shaderContainer[i].pixelShader);
        SAFE_RELEASE(m_shaderContainer[i].inputLayout);
    }
}

// 定数バッファをシェーダーコンテナに登録する関数
void ShaderManager::RegisterCB(ShaderType shaderType, UINT slot, ID3D11Buffer** ppBuffer)
{
    size_t index = static_cast<size_t>(shaderType);
    if (slot < m_shaderContainer[index].constantBuffers.size()) {
        m_shaderContainer[index].constantBuffers[slot] = ppBuffer;
    }
}

// シェーダーをバインドする関数
void ShaderManager::BindShader(ShaderType shaderType)
{
    size_t index = static_cast<size_t>(shaderType);

    m_pContext->VSSetShader(m_shaderContainer[index].vertexShader, nullptr, 0);
    m_pContext->PSSetShader(m_shaderContainer[index].pixelShader, nullptr, 0);
    m_pContext->IASetInputLayout(m_shaderContainer[index].inputLayout);

    for (UINT slot = 0; slot < 15; slot++) {
        if (m_shaderContainer[index].constantBuffers[slot]) {
            m_pContext->VSSetConstantBuffers(slot, 1, m_shaderContainer[index].constantBuffers[slot]);
            m_pContext->PSSetConstantBuffers(slot, 1, m_shaderContainer[index].constantBuffers[slot]);
         }
    }
}

// -------------------------- Bind Buffer

// TransformBufferを更新する関数
void ShaderManager::UpdateTransformCB(const TransformBuffer& transformData)
{
    m_pContext->UpdateSubresource(m_transformCB, 0, nullptr, &transformData, 0, 0);
}
// CameraBufferを更新する関数
void ShaderManager::UpdateCameraCB(const CameraBuffer& cameraData)
{
    m_pContext->UpdateSubresource(m_cameraCB, 0, nullptr, &cameraData, 0, 0);
}
// 任意の定数バッファを更新する関数
void ShaderManager::UpdateOtherCB(UINT slot, const void* data, size_t dataSize)
{

}

//----------------------------------------------------------------------------- private

// VertexShaderの読み込み
bool ShaderManager::LoadVertexShader(const char* filename, ID3D11VertexShader** ppVertexShader, VsBinaryData& vbData)
{
    std::ifstream ifs_vs(filename, std::ios::binary);
    if (!ifs_vs)return false;

    // ファイルサイズを取得
    ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
    vbData.fileSize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
    ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

    // バイナリデータを格納するためのバッファを確保
    vbData.vsBinaryPointer = new unsigned char[vbData.fileSize];

    ifs_vs.read((char*)vbData.vsBinaryPointer, vbData.fileSize); // バイナリデータを読み込む
    ifs_vs.close(); // ファイルを閉じる

    // 頂点シェーダーの作成
    HRESULT hr = m_pDevice->CreateVertexShader(vbData.vsBinaryPointer, vbData.fileSize, nullptr, ppVertexShader);
    if (FAILED(hr)) {
        delete[] vbData.vsBinaryPointer; // メモリリークしないようにバイナリデータのバッファを解放
        return false;
    }

    return true;
}

// PixelShaderの読み込み
bool ShaderManager::LoadPixelShader(const char* filename, ID3D11PixelShader** ppPixelShader)
{
    // 事前コンパイル済みピクセルシェーダーの読み込み
    std::ifstream ifs_ps(filename, std::ios::binary);
    if (!ifs_ps)return false;

    // ファイルサイズを取得
    ifs_ps.seekg(0, std::ios::end);
    std::streamsize filesize = ifs_ps.tellg();
    ifs_ps.seekg(0, std::ios::beg);

    // バイナリデータを格納するためのバッファを確保
    unsigned char* psbinary_pointer = new unsigned char[filesize];
    ifs_ps.read((char*)psbinary_pointer, filesize);
    ifs_ps.close();

    // ピクセルシェーダーの作成
    HRESULT hr = m_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, ppPixelShader);

    delete[] psbinary_pointer; // バイナリデータのバッファを解放

    if (FAILED(hr))return false;

    return true;
}
