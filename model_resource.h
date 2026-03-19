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

class aiScene;

struct ModelMesh {
	std::string name;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	unsigned int numVertices;
	unsigned int numIndices;
};

struct ModelTexture {
	std::string name;
	ComPtr<ID3D11ShaderResourceView> texture;
};

class ModelResource {
public:
    const aiScene* AiScene = nullptr;

	std::string filePath;
	std::vector<ModelMesh> meshes;
	std::vector<ModelTexture> textures;

};
#endif // MODEL_RESOURCE_H