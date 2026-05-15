//===================================================
// shader_repository.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/05/15
//===================================================
#include "shader_repository.h"

#include <memory>
#include "Engine/Device/direct3d.h"

#include "Utility/debug_ostream.h"

#include <fstream>

// シェーダーリポジトリの初期化
void ShaderRepository::Initialize()
{
    m_pContext = Direct3D_GetDeviceContext();
    m_pDevice = Direct3D_GetDevice();
    m_shaderCache.clear();

    // シェーダーファミリーごとのベースシェーダーリソースを生成
    {
        ShaderProgramResource litShader;
        litShader.name = SHADER_BASE_NAMES[static_cast<int>(ShaderBase::Lit)];
        litShader.shaderBase = ShaderBase::Lit;
        litShader.vertexShader = GenerateVertexShaderResource("lit_vs.cso", VertexType::Lit);
        litShader.pixelShader = GeneratePixelShaderResource("lit_ps.cso");
        GenerateShaderProgramResource(litShader);

        ShaderProgramResource skinnedLitShader;
        skinnedLitShader.name = SHADER_BASE_NAMES[static_cast<int>(ShaderBase::SkinnedLit)];
        skinnedLitShader.shaderBase = ShaderBase::SkinnedLit;
        skinnedLitShader.vertexShader = GenerateVertexShaderResource("skinned_lit_vs.cso", VertexType::SkinnedLit);
        skinnedLitShader.pixelShader = litShader.pixelShader; // ライト付きシェーダーと同じピクセルシェーダーを使用
        GenerateShaderProgramResource(skinnedLitShader);


        ShaderProgramResource unlitShader;
        unlitShader.name = SHADER_BASE_NAMES[static_cast<int>(ShaderBase::Unlit)];
        unlitShader.shaderBase = ShaderBase::Unlit;
        unlitShader.vertexShader = GenerateVertexShaderResource("unlit_vs.cso", VertexType::Unlit);
        unlitShader.pixelShader = GeneratePixelShaderResource("unlit_ps.cso");
        GenerateShaderProgramResource(unlitShader);

        ShaderProgramResource spriteShader;
        spriteShader.name = SHADER_BASE_NAMES[static_cast<int>(ShaderBase::Sprite)];
        spriteShader.shaderBase = ShaderBase::Sprite;
        spriteShader.vertexShader = GenerateVertexShaderResource("sprite_vs.cso", VertexType::Sprite);
        spriteShader.pixelShader = GeneratePixelShaderResource("sprite_ps.cso");
        GenerateShaderProgramResource(spriteShader);
    }
}

// シェーダーリポジトリの終了処理
void ShaderRepository::Finalize()
{
    m_shaderCache.clear();
}

// ----------------------------- public リソース生成
#pragma region シェーダーリソース生成
// シェーダープログラムリソースの生成
ShaderProgramResource* ShaderRepository::GenerateShaderProgramResource(const ShaderProgramResource& shader)
{
    // キャッシュを確認し、既に存在する場合は上書きする
    auto it = m_shaderCache.find(shader.name);
    if (it != m_shaderCache.end())
    {
        it->second = std::make_unique<ShaderProgramResource>(shader);
        return it->second.get();
    }

    // キャッシュに存在しない場合は新規に追加する
    m_shaderCache[shader.name] = std::make_unique<ShaderProgramResource>(shader);
    return m_shaderCache[shader.name].get();
}

// 頂点シェーダーリソースの生成
VertexShaderResource* ShaderRepository::GenerateVertexShaderResource(const std::string& filePath, VertexType vertexType)
{
    // キャッシュを確認し、既に存在する場合は上書きする
    auto it = m_vertexShaderCache.find(filePath);
    if (it != m_vertexShaderCache.end())
    {
        it->second = std::make_unique<VertexShaderResource>();
        it->second->filePath = filePath;
        it->second->vertexType = vertexType;
        VsBinaryData vbData;
        LoadVertexShader(&(it->second->vertexShader), filePath, vbData);
        CreateInputLayout(&(it->second->inputLayout), vertexType, vbData);
        return it->second.get();
    }

    // キャッシュに存在しない場合は新規に追加する
    m_vertexShaderCache[filePath] = std::make_unique<VertexShaderResource>();
    m_vertexShaderCache[filePath]->filePath = filePath;
    m_vertexShaderCache[filePath]->vertexType = vertexType;
    VsBinaryData vbData;
    LoadVertexShader(&(m_vertexShaderCache[filePath]->vertexShader), filePath, vbData);
    CreateInputLayout(&(m_vertexShaderCache[filePath]->inputLayout), vertexType, vbData);
    return m_vertexShaderCache[filePath].get();
}

// ピクセルシェーダーリソースの生成
PixelShaderResource* ShaderRepository::GeneratePixelShaderResource(const std::string& filePath)
{
    // キャッシュを確認し、既に存在する場合は上書きする
    auto it = m_pixelShaderCache.find(filePath);
    if (it != m_pixelShaderCache.end())
    {
        it->second = std::make_unique<PixelShaderResource>();
        it->second->filePath = filePath;
        LoadPixelShader(&(it->second->pixelShader), filePath);
        return it->second.get();
    }

    // キャッシュに存在しない場合は新規に追加する
    m_pixelShaderCache[filePath] = std::make_unique<PixelShaderResource>();
    m_pixelShaderCache[filePath]->filePath = filePath;
    LoadPixelShader(&(m_pixelShaderCache[filePath]->pixelShader), filePath);
    return m_pixelShaderCache[filePath].get();
}

// 定数バッファリソースの生成
ConstantBufferResource* ShaderRepository::GenerateConstantBufferResource(
    const std::string& name, UINT slot, UINT byteWidth, bool bindVs, bool bindPs, ConstantBufferUsage usage)
{
    // キャッシュを確認し、既に存在する場合は上書きする
    auto it = m_constantBufferCache.find(name);
    if (it != m_constantBufferCache.end())
    {
        it->second = std::make_unique<ConstantBufferResource>();
        it->second->name = name;
        it->second->slot = slot;
        it->second->byteWidth = byteWidth;
        it->second->bindVS = bindVs;
        it->second->bindPS = bindPs;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.ByteWidth = byteWidth;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (usage == ConstantBufferUsage::Dynamic) {
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        }
        else if (usage == ConstantBufferUsage::Default) {
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.CPUAccessFlags = 0;
        }

        HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &it->second->buffer);
        if (FAILED(hr)) {
            m_constantBufferCache.erase(name); // 作成に失敗した場合はキャッシュから削除
            return nullptr;
        }

        return it->second.get();
    }

    // キャッシュに存在しない場合は新規に追加する
    m_constantBufferCache[name] = std::make_unique<ConstantBufferResource>();
    m_constantBufferCache[name]->name = name;
    m_constantBufferCache[name]->slot = slot;
    m_constantBufferCache[name]->byteWidth = byteWidth;
    m_constantBufferCache[name]->bindVS = bindVs;
    m_constantBufferCache[name]->bindPS = bindPs;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = byteWidth;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (usage == ConstantBufferUsage::Dynamic) {
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else if (usage == ConstantBufferUsage::Default) {
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
    }

    HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_constantBufferCache[name]->buffer);
    if (FAILED(hr)) {
        m_constantBufferCache.erase(name); // 作成に失敗した場合はキャッシュから削除
        return nullptr;
    }

    return m_constantBufferCache[name].get();
}

#pragma endregion

// ----------------------------- private シェーダー読み込み
#pragma region シェーダー読み込み
// 頂点シェーダーの読み込み
bool ShaderRepository::LoadVertexShader(ID3D11VertexShader** outVs, const std::string& filePath, VsBinaryData& vbData)
{
    std::ifstream ifs_vs(filePath, std::ios::binary);
    if (!ifs_vs)return false;

    // ファイルサイズを取得
    ifs_vs.seekg(0, std::ios::end);
    vbData.fileSize = ifs_vs.tellg();
    ifs_vs.seekg(0, std::ios::beg);

    // バイナリデータを読み込む
    vbData.vsBinaryPointer = new unsigned char[vbData.fileSize];
    ifs_vs.read((char*)vbData.vsBinaryPointer, vbData.fileSize);
    ifs_vs.close();

    // 頂点シェーダーの作成
    HRESULT hr = m_pDevice->CreateVertexShader(vbData.vsBinaryPointer, vbData.fileSize, nullptr, outVs);
    if (FAILED(hr)) {
        delete[] vbData.vsBinaryPointer;
        return false;
    }

    return true;
}

// 頂点レイアウトの作成
bool ShaderRepository::CreateInputLayout(ID3D11InputLayout** outInputLayout, VertexType vertexType, const VsBinaryData& vbData)
{
    // 頂点レイアウトの定義
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
    
    switch (vertexType) {
    case VertexType::Lit:
        layout.resize(6);
        layout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[1] = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[2] = { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[3] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[4] = { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[5] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        break;
    case VertexType::SkinnedLit:
        layout.resize(8);
        layout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[1] = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[2] = { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[3] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[4] = { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[5] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[6] = { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[7] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        break;
    case VertexType::Unlit:
        layout.resize(4);
        layout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[1] = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[2] = { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[3] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        break;
    case VertexType::Sprite:
        layout.resize(3);
        layout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[1] = { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        layout[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        break;
    default: return false; break;
    }
    UINT num_elements = layout.size();

    // 頂点レイアウトの作成
    HRESULT hr = m_pDevice->CreateInputLayout(&(layout[0]), num_elements, vbData.vsBinaryPointer, vbData.fileSize, outInputLayout);
    delete[] vbData.vsBinaryPointer;
    if (FAILED(hr)) {
        hal::dout << "ShaderManager::Initialize() : SpriteShaderの頂点レイアウトの作成に失敗しました" << std::endl;
        return false;
    }

    return true;
}

// ピクセルシェーダーの読み込み
bool ShaderRepository::LoadPixelShader(ID3D11PixelShader** outPs, const std::string& filePath)
{
    // 事前コンパイル済みピクセルシェーダーの読み込み
    std::ifstream ifs_ps(filePath, std::ios::binary);
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
    HRESULT hr = m_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, outPs);
    delete[] psbinary_pointer; // バイナリデータのバッファを解放
    if (FAILED(hr))return false;

    return true;
}


#pragma endregion