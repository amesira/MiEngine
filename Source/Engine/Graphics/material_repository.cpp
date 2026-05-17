//===================================================
// material_repository.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/29
//===================================================
#include "material_repository.h"

#include <memory>
#include "Engine/Device/direct3d.h"

#include "Utility/debug_ostream.h"
#include "Engine/engine_service_locator.h"

#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

// マテリアルリポジトリの初期化
void MaterialRepository::Initialize()
{
    m_pDevice = Direct3D_GetDevice();
    m_pContext = Direct3D_GetDeviceContext();

    m_materialCache.clear();

    // 定数バッファの作成
    m_materialCB = SHADER_REPOSITORY->GenerateConstantBufferResource(
        "MaterialBuffer",
        8,
        sizeof(MaterialBufferData),
        true,
        true,
        ConstantBufferUsage::Dynamic);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::Lit)], m_materialCB);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::SkinnedLit)], m_materialCB);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::Unlit)], m_materialCB);

    m_customPropertyCB = SHADER_REPOSITORY->GenerateConstantBufferResource(
        "CustomPropertyBuffer",
        9,
        sizeof(XMFLOAT4) * MaterialResource::CUSTOM_PROPERTY_COUNT,
        true,
        true,
        ConstantBufferUsage::Dynamic);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::Lit)], m_customPropertyCB);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::SkinnedLit)], m_customPropertyCB);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::Unlit)], m_customPropertyCB);

    // デフォルトテクスチャの作成
    m_defaultAlbedoTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\default_albedo.png");
    m_defaultNormalTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\default_normal.png");
    m_defaultEmissiveTexture = m_defaultAlbedoTexture;
    m_defaultAOTexture = m_defaultAlbedoTexture;

    // デフォルトマテリアルの作成
    MaterialResource defaultMaterial;
    defaultMaterial.name = "default";
    defaultMaterial.albedoTexture = m_defaultAlbedoTexture;
    defaultMaterial.normalTexture = m_defaultNormalTexture;
    defaultMaterial.emissiveTexture = m_defaultEmissiveTexture;
    defaultMaterial.aoTexture = m_defaultAOTexture;
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
    m_pContext->Map(m_materialCB->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    MaterialBufferData* bufferData = reinterpret_cast<MaterialBufferData*>(msr.pData);
    *bufferData = material;
    m_pContext->Unmap(m_materialCB->buffer.Get(), 0);
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
        m_defaultNormalTexture->texture.GetAddressOf());

    m_pContext->PSSetShaderResources(2, 1, material.emissiveTexture ?
        material.emissiveTexture->texture.GetAddressOf() :
        m_defaultEmissiveTexture->texture.GetAddressOf());

    m_pContext->PSSetShaderResources(3, 1, material.aoTexture ?
        material.aoTexture->texture.GetAddressOf() :
        m_defaultAOTexture->texture.GetAddressOf());
}

void MaterialRepository::BindCustomProperties(XMFLOAT4* customPropaties)
{
    // カスタムプロパティをGPUに転送
    D3D11_MAPPED_SUBRESOURCE msr = {};
    m_pContext->Map(m_customPropertyCB->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    XMFLOAT4* cbData = reinterpret_cast<XMFLOAT4*>(msr.pData);
    for (int i = 0; i < MaterialResource::CUSTOM_PROPERTY_COUNT; i++)
    {
        cbData[i] = customPropaties[i];
    }
    m_pContext->Unmap(m_customPropertyCB->buffer.Get(), 0);
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