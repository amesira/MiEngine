//===================================================
// material_repository.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/29
//===================================================
#include "material_repository.h"

#include <memory>
#include "DirectXTex.h"

#include "debug_ostream.h"
#include "engine_service_locator.h"

// マテリアルリポジトリの初期化
void MaterialRepository::Initialize()
{
    m_pDevice = Direct3D_GetDevice();
    m_pContext = Direct3D_GetDeviceContext();

    m_materialCache.clear();

    // 定数バッファの作成
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 頻繁に更新する予定なのでDynamicにする
    bufferDesc.ByteWidth = sizeof(MaterialBufferData);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_materialBuffer);

    auto shader = EngineServiceLocator::GetShaderManager();
    if (shader) {
        shader->RegisterCB(ShaderManager::ShaderType::Lit, 9, &m_materialBuffer);
        shader->RegisterCB(ShaderManager::ShaderType::SkinnedLit, 9, &m_materialBuffer);
    }

    // デフォルトテクスチャの作成
    m_defaultAlbedoTexture = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Material\\def_albedo_texture.bmp");

    // デフォルトマテリアルの作成
    MaterialResource defaultMaterial;
    defaultMaterial.name = "default";
    defaultMaterial.albedoTexture = m_defaultAlbedoTexture;
    GenerateMaterial(defaultMaterial);
}

// マテリアルリポジトリの終了処理
void MaterialRepository::Finalize()
{
    m_materialCache.clear();
}

// マテリアルの生成
MaterialResource* MaterialRepository::GenerateMaterial(const MaterialResource& material)
{
    // キャッシュを確認し、既に存在する場合は上書きする
    auto it = m_materialCache.find(material.name);
    if (it != m_materialCache.end())
    {
        it->second = std::make_unique<MaterialResource>(material);
        return it->second.get();
    }

    // キャッシュに存在しない場合は新規に追加する
    m_materialCache[material.name] = std::make_unique<MaterialResource>(material);
    return m_materialCache[material.name].get();
}

// マテリアルの取得。キャッシュに無い場合は読み込む。
MaterialResource* MaterialRepository::GetMaterial(const std::string& filePath)
{
    // キャッシュを確認
    auto it = m_materialCache.find(filePath);
    if (it != m_materialCache.end())
    {
        return it->second.get();
    }

    // キャッシュに無い場合は読み込む
    return LoadMaterial(filePath);
}

// マテリアルのバインド
void MaterialRepository::BindMaterialCB(const MaterialBufferData& material)
{
    // MaterialBuffferDataとしてGPUにデータ転送
    D3D11_MAPPED_SUBRESOURCE msr = {};
    m_pContext->Map(m_materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    MaterialBufferData* bufferData = reinterpret_cast<MaterialBufferData*>(msr.pData);
    *bufferData = material;
    m_pContext->Unmap(m_materialBuffer, 0);
}

// マテリアルのバインド（テクスチャ）
void MaterialRepository::BindMaterialTexture(const MaterialResource& material)
{
    // t0～t3にテクスチャをセット
    m_pContext->PSSetShaderResources(0, 1, material.albedoTexture ?
        material.albedoTexture->texture.GetAddressOf() :
        m_defaultAlbedoTexture->texture.GetAddressOf());
    m_pContext->PSSetShaderResources(1, 1, material.normalTexture ?
        material.normalTexture->texture.GetAddressOf() :
        m_defaultAlbedoTexture->texture.GetAddressOf());
    m_pContext->PSSetShaderResources(2, 1, material.emissiveTexture ?
        material.emissiveTexture->texture.GetAddressOf() :
        m_defaultAlbedoTexture->texture.GetAddressOf());
    m_pContext->PSSetShaderResources(3, 1, material.aoTexture ?
        material.aoTexture->texture.GetAddressOf() :
        m_defaultAlbedoTexture->texture.GetAddressOf());
}

//-------------------------------------

// マテリアルの読み込み
MaterialResource* MaterialRepository::LoadMaterial(const std::string& filePath)
{
    m_materialCache[filePath] = std::make_unique<MaterialResource>();
    MaterialResource* materialResource = m_materialCache[filePath].get();
    materialResource->name = filePath;

    return materialResource;
}

// マテリアルの解放
void MaterialRepository::ReleaseMaterial(const std::string& filePath)
{
    auto it = m_materialCache.find(filePath);
    if (it != m_materialCache.end())
    {
        // ComPtrが自動的にリソースを解放するので、特になし
        m_materialCache.erase(it);
    }
}