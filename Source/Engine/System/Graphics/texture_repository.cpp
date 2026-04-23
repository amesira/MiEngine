//===================================================
// texture_repository.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/29
//===================================================
#include "texture_repository.h"

#include <memory>
#include "Engine/System/Device/direct3d.h"

#include "Utility/debug_ostream.h"

// テクスチャリポジトリの初期化
void TextureRepository::Initialize() 
{
    m_pDevice = Direct3D_GetDevice();
    m_pContext = Direct3D_GetDeviceContext();

    m_textureCache.clear();
}

// テクスチャリポジトリの終了処理
void TextureRepository::Finalize() 
{
    m_textureCache.clear();
}

// テクスチャの生成
TextureResource* TextureRepository::GenerateTextureResource(const TextureResource& texture)
{
    // キャッシュを確認し、既に存在する場合は上書きする
    auto it = m_textureCache.find(texture.name);
    if (it != m_textureCache.end())
    {
        it->second = std::make_unique<TextureResource>(texture);
        return it->second.get();
    }

    // キャッシュに存在しない場合は新規に追加する
    m_textureCache[texture.name] = std::make_unique<TextureResource>(texture);
    return m_textureCache[texture.name].get();
}

// テクスチャの取得。キャッシュに無い場合は読み込む。
TextureResource* TextureRepository::GetTextureResource(const std::wstring& filePath)
{
    // キャッシュを確認
    auto it = m_textureCache.find(filePath);
    if (it != m_textureCache.end())
    {
        return it->second.get();
    }

    // キャッシュに無い場合は読み込む
    return LoadTexture(filePath);
}

//-------------------------------------

// テクスチャの読み込み
TextureResource* TextureRepository::LoadTexture(const std::wstring& filePath)
{
    TextureResource* textureResource = new TextureResource();
    textureResource->name = filePath;
    
    // テクスチャの読み込み
    TexMetadata metadata;
    ScratchImage image;
    LoadFromWICFile(filePath.c_str(), WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metadata, textureResource->texture.GetAddressOf());

    // 読み込み失敗時のエラーチェック
    if (textureResource->texture == nullptr)
    {
        delete textureResource;
        return nullptr;
    }

    m_textureCache[filePath] = std::make_unique<TextureResource>(*textureResource);
    return textureResource;
}

// テクスチャの解放
void TextureRepository::ReleaseTexture(const std::wstring& filePath)
{
    auto it = m_textureCache.find(filePath);
    if (it != m_textureCache.end())
    {
        // ComPtrが自動的にリソースを解放するので、特になし
        m_textureCache.erase(it);
    }
}