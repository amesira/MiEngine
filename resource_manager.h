//---------------------------------------------------
// resource_manager.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//---------------------------------------------------
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "texture_repository.h"
#include "model_repository.h"
#include "material_repository.h"

class ResourceManager {
private:
    // テクスチャリポジトリ
    TextureRepository m_textureRepository;
    // モデルリポジトリ
    ModelRepository m_modelRepository;
    // マテリアルリポジトリ
    MaterialRepository m_materialRepository;

public:
    // 初期化
    void Initialize();
    // 終了処理
    void Finalize();

    // テクスチャリポジトリへのアクセス
    TextureRepository* GetTextureRepository() { return &m_textureRepository; }
    // モデルリポジトリへのアクセス
    ModelRepository* GetModelRepository() { return &m_modelRepository; }
    // マテリアルリポジトリへのアクセス
    MaterialRepository* GetMaterialRepository() { return &m_materialRepository; }

};

#endif // RESOURCE_MANAGER_H