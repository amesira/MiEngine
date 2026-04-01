//===================================================
// texture_repository.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/29
//===================================================
#include "texture_repository.h"

#include "shader.h"
#include <memory>
#include "DirectXTex.h"

#include "debug_ostream.h"

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
    for (auto& pair : m_textureCache)
    {
        ReleaseTexture(pair.first);
    }
    m_textureCache.clear();
}

// テクスチャの取得。キャッシュに無い場合は読み込む。
TextureResource* TextureRepository::GetTexture(const std::string& filePath)
{
    // キャッシュを確認
    auto it = m_textureCache.find(filePath);
    if (it != m_textureCache.end())
    {
        return it->second.get();
    }
    // キャッシュに無い場合は読み込む
    LoadTexture(filePath);
    return m_textureCache[filePath].get();
}

//-------------------------------------

// テクスチャの読み込み
void TextureRepository::LoadTexture(const std::string& filePath)
{
    m_textureCache[filePath] = std::make_unique<TextureResource>();
    TextureResource* textureResource = m_textureCache[filePath].get();
    textureResource->filePath = filePath;
    
    // テクスチャの読み込み
    TexMetadata metadata;
    ScratchImage image;
    LoadFromWICFile(std::wstring(filePath.begin(), filePath.end()).c_str(), WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metadata, textureResource->texture.GetAddressOf());

    // 読み込み失敗時のエラーチェック
    //assert(textureResource->texture.Get());
}

// テクスチャの解放
void TextureRepository::ReleaseTexture(const std::string& filePath)
{
    auto it = m_textureCache.find(filePath);
    if (it != m_textureCache.end())
    {
        // ComPtrが自動的にリソースを解放するので、特になし
        m_textureCache.erase(it);
    }
}