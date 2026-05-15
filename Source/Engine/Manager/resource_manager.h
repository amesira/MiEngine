//---------------------------------------------------
// resource_manager.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//---------------------------------------------------
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "Engine/Graphics/texture_repository.h"
#include "Engine/Graphics/model_repository.h"
#include "Engine/Graphics/material_repository.h"
#include "Engine/Graphics/shader_repository.h"

class ResourceManager {
private:
    // テクスチャリポジトリ
    TextureRepository m_textureRepository;
    // シェーダーリポジトリ
    ShaderRepository m_shaderRepository;
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
    // シェーダーリポジトリへのアクセス
    ShaderRepository* GetShaderRepository() { return &m_shaderRepository; }
    // モデルリポジトリへのアクセス
    ModelRepository* GetModelRepository() { return &m_modelRepository; }
    // マテリアルリポジトリへのアクセス
    MaterialRepository* GetMaterialRepository() { return &m_materialRepository; }

};

#endif // RESOURCE_MANAGER_H