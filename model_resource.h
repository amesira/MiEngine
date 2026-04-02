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

// モデルリソース
class ModelResource {
public:
    const aiScene* AiScene = nullptr;
	std::string name;

    // メッシュのリスト
	std::vector<ModelMesh> meshes;
    // モデル標準となるマテリアルリソースのリスト
    std::vector<MaterialResource*> materialResources;

};
#endif // MODEL_RESOURCE_H