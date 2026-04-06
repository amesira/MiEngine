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
#include <vector>
#include <string>

#include <DirectXMath.h>
using namespace DirectX;

#include "assimp/matrix4x4.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

struct LitVertex;
struct SkinnedLitVertex;

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
    void BindSkinningCB(const std::vector<XMMATRIX>& boneMatrix);

private:
    // モデルの読み込み
    ModelResource* LoadModel(const std::string& filePath);
    // Assimpの行列をXMMATRIXに変換
    XMMATRIX AssimpMatToXMMatrix(const aiMatrix4x4& m);

    // aiMeshから頂点バッファを作成
    void SetLitVertexInfo(LitVertex* vertices, const aiMesh* mesh);
    // aiMeshからスキニング頂点バッファを作成
    void SetSkinnedLitVertexInfo(SkinnedLitVertex* vertices, const aiMesh* mesh, const std::unordered_map<std::string, unsigned int>& boneNameToIndex);

    // aiMaterialからMaterialResourceを作成
    MaterialResource CreateMaterialResource(aiMaterial* mat);

    // モデルの解放
    void ReleaseModel(const std::string& filePath);

};

#endif // MODEL_REPOSITORY_H