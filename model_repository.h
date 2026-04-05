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

#include "assimp/matrix4x4.h"
struct aiNode;

class ModelRepository {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // モデルリソースのキャッシュ
    std::unordered_map<std::string, std::unique_ptr<ModelResource>> m_modelCache;

    // スキニングバッファ
    ComPtr<ID3D11Buffer> m_skinningBuffer;


public:
    // 初期化
    void Initialize();
    // 終了
    void Finalize();

    // モデルの取得。キャッシュに無い場合は読み込む。
    ModelResource* GetModel(const std::string& filePath);

    // スキニングCBのバインド
    void BindSkinningCB(const std::vector<ModelBone>& bones);

private:
    // モデルの読み込み
    ModelResource* LoadModel(const std::string& filePath);
    // 
    XMMATRIX AssimpMatToXMMatrix(const aiMatrix4x4& m);

    // モデルの解放
    void ReleaseModel(const std::string& filePath);

};

#endif // MODEL_REPOSITORY_H