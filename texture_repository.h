//---------------------------------------------------
// texture_repository.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/29
//---------------------------------------------------
#ifndef TEXTURE_REPOSITORY_H
#define TEXTURE_REPOSITORY_H
#include "texture_resource.h"

#include <unordered_map>
#include <memory>

class TextureRepository {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // テクスチャリソースのキャッシュ
    std::unordered_map<std::string, std::unique_ptr<TextureResource>> m_textureCache;

public:
    // 初期化
    void Initialize();
    // 終了
    void Finalize();

    // テクスチャの取得。キャッシュに無い場合は読み込む。
    TextureResource* GetTexture(const std::string& filePath);

private:
    // テクスチャの読み込み
    void LoadTexture(const std::string& filePath);

    // テクスチャの解放
    void ReleaseTexture(const std::string& filePath);
};

#endif // TEXTURE_REPOSITORY_H