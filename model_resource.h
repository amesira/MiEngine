//---------------------------------------------------
// model_resource.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//---------------------------------------------------
#ifndef MODEL_RESOURCE_H
#define MODEL_RESOURCE_H
#include <string>

#include "d3d11.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "direct3d.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include "texture_resource.h"
#include "material_resource.h"

class aiScene;

// モデルのメッシュデータ
struct ModelMesh {
	std::string name;

    // 頂点バッファとインデックスバッファ
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	unsigned int numVertices;
	unsigned int numIndices;

    // マテリアルインデックス
	uint32_t materialIndex;
};

// ボーンのデータ
struct ModelBone {
	std::string name;
    unsigned int index;

    // ボーンのオフセット行列（モデル空間からボーン空間への変換行列）
	XMMATRIX offsetMatrix;
    XMMATRIX globalTransform;
    XMMATRIX finalTransform;
};

// モデルリソース
class ModelResource {
private: friend class ModelRepository;
    const aiScene* AiScene = nullptr;

public:
	std::string name;

    // メッシュのリスト
	std::vector<ModelMesh> meshes;
    // モデル標準となるマテリアルリソースのリスト
    std::vector<MaterialResource*> materialResources;

    // ボーンのリスト
    std::vector<ModelBone> bones;
    std::unordered_map<std::string, unsigned int> boneNameToIndex;

    // 頂点の種類
    enum class VertexType {
        Lit,
        SkinnedLit
    } vertexType = VertexType::Lit;

};
#endif // MODEL_RESOURCE_H