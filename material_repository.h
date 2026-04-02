//---------------------------------------------------
// material_repository.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/29
//---------------------------------------------------
#ifndef MATERIAL_REPOSITORY_H
#define MATERIAL_REPOSITORY_H
#include "material_resource.h"

#include <unordered_map>
#include <memory>

class MaterialRepository {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // デフォルトテクスチャリソース
    TextureResource* m_defaultAlbedoTexture = nullptr;
    TextureResource* m_defaultNormalTexture = nullptr;
    TextureResource* m_defaultEmissiveTexture = nullptr;
    TextureResource* m_defaultAOTexture = nullptr;

    // マテリアルリソースのキャッシュ
    std::unordered_map<std::string, std::unique_ptr<MaterialResource>> m_materialCache;

    // マテリアルの定数バッファ
    ID3D11Buffer* m_materialBuffer = nullptr;

public:
    // 初期化
    void Initialize();
    // 終了
    void Finalize();

    // マテリアルの生成。
    MaterialResource* GenerateMaterial(const MaterialResource& material);
    // マテリアルの取得。キャッシュに無い場合は読み込む。
    MaterialResource* GetMaterial(const std::string& filePath);

    // マテリアルのバインド
    void BindMaterialCB(const MaterialBufferData& material);
    void BindMaterialTexture(const MaterialResource& material);

private:
    // マテリアルの読み込み
    // Material/wood.mat.jsonのようなファイルパスにする予定。
    MaterialResource* LoadMaterial(const std::string& filePath);

    // マテリアルの解放
    void ReleaseMaterial(const std::string& filePath);
};

#endif // MATERIAL_REPOSITORY_H