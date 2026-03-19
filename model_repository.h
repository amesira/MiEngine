//---------------------------------------------------
// model_repository.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//---------------------------------------------------
#ifndef MODEL_REPOSITORY_H
#define MODEL_REPOSITORY_H
#include "model_resource.h"

#include <unordered_map>
#include <memory>

class ModelRepository {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // モデルリソースのキャッシュ
    std::unordered_map<std::string, std::unique_ptr<ModelResource>> m_modelCache;

public:
    // 初期化
    void Initialize();
    // 終了
    void Finalize();

    // モデルの取得。キャッシュに無い場合は読み込む。
    ModelResource* GetModel(const std::string& filePath);

private:
    // モデルの読み込み
    void LoadModel(const std::string& filePath);

    // モデルの解放
    void ReleaseModel(const std::string& filePath);

};

#endif // MODEL_REPOSITORY_H