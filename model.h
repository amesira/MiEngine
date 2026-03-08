// model.h
// DX授業配布

#pragma once
//#define NOMINMAX
#include <unordered_map>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

#include	"d3d11.h"
#include	"DirectXMath.h"
using namespace DirectX;
#include	"direct3d.h"

#define MODEL_MAX_INSTANCE (512)

struct MODEL
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer;
	ID3D11Buffer** IndexBuffer;

	// インスタンスバッファ
    ID3D11Buffer* InstanceBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;
};

void Model_Initialize();
void Model_Finalize();

MODEL* ModelLoad(const char* FileName);
void ModelRelease(MODEL* model);

void ModelDraw(MODEL* model);