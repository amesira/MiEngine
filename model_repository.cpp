//===================================================
// model_repository.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//===================================================
#include "model_repository.h"

#include <memory>
#include "mi_string.h"

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

#include "engine_service_locator.h"

#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

// モデルリポジトリの初期化
void ModelRepository::Initialize() 
{
    m_pDevice = Direct3D_GetDevice();
    m_pContext = Direct3D_GetDeviceContext();

    m_modelCache.clear();
}

// モデルリポジトリの終了処理
void ModelRepository::Finalize() 
{
    for (auto& pair : m_modelCache)
    {
        ReleaseModel(pair.first);
    }
    m_modelCache.clear();
}

// モデルの取得。キャッシュに無い場合は読み込む。
ModelResource* ModelRepository::GetModel(const std::string& filePath)
{
    // キャッシュを確認
    auto it = m_modelCache.find(filePath);
    if (it != m_modelCache.end())
    {
        return it->second.get();
    }

    // キャッシュに無い場合は読み込む
    return LoadModel(filePath);
}

//------------------------------------

// モデルの読み込み
ModelResource* ModelRepository::LoadModel(const std::string& filePath)
{
    const aiScene* scene = aiImportFile(
        filePath.c_str(), 
        aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

    // 読み込みエラーチェック
    if (scene == nullptr)
    {
        return nullptr;
    }

    // モデルリソースを作成してキャッシュに追加
    m_modelCache[filePath] = std::make_unique<ModelResource>();
    ModelResource* model = m_modelCache[filePath].get();
    model->name = filePath;
    model->AiScene = scene;
    model->meshes.reserve(scene->mNumMeshes);
    model->materialResources.resize(scene->mNumMaterials);

    // メッシュの読み込み
    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh* mesh = scene->mMeshes[m];
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        // ModelMesh構造体にデータを格納
        ModelMesh& modelMesh = model->meshes.emplace_back();
        modelMesh.name = mesh->mName.C_Str();
        modelMesh.numVertices = mesh->mNumVertices;
        modelMesh.numIndices = mesh->mNumFaces * 3;
        modelMesh.materialIndex = mesh->mMaterialIndex;

        // 頂点バッファ生成
        {
            LitVertex* vertex = new LitVertex[mesh->mNumVertices];

            for (unsigned int v = 0; v < mesh->mNumVertices; v++)
            {
                vertex[v].position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
                vertex[v].texCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
                vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
                vertex[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            }

            D3D11_BUFFER_DESC bd = {};
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = sizeof(LitVertex) * mesh->mNumVertices;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            D3D11_SUBRESOURCE_DATA sd = {};
            sd.pSysMem = vertex;

            m_pDevice->CreateBuffer(&bd, &sd, &modelMesh.vertexBuffer);

            delete[] vertex;
        }

        // インデックスバッファ生成
        {
            unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

            for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            {
                const aiFace* face = &mesh->mFaces[f];
                assert(face->mNumIndices == 3);
                index[f * 3 + 0] = face->mIndices[0];
                index[f * 3 + 1] = face->mIndices[1];
                index[f * 3 + 2] = face->mIndices[2];
            }

            D3D11_BUFFER_DESC bd = {};
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
            bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
            bd.CPUAccessFlags = 0;

            D3D11_SUBRESOURCE_DATA sd = {};
            sd.pSysMem = index;
            m_pDevice->CreateBuffer(&bd, &sd, &modelMesh.indexBuffer);

            delete[] index;
        }

        // マテリアルリソースの生成
        {
            XMFLOAT4 albedoColor = { 1.0f,1.0f,1.0f,1.0f };
            std::wstring albedoTexturePath;

            // マテリアルからベースカラーを取得
            {
                aiColor4D c;
                if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_BASE_COLOR, &c) ||
                    AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &c))
                {
                    albedoColor = { c.r, c.g, c.b, c.a };
                }
            }

            // マテリアルからテクスチャパスを取得
            if (mat->GetTextureCount(aiTextureType_BASE_COLOR) > 0 ||
                mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString texturePath;
                if (AI_SUCCESS == mat->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath) ||
                    AI_SUCCESS == mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
                {
                    albedoTexturePath = MiString::ToWString(texturePath.C_Str());
                }
            }

            // 標準マテリアルを生成
            MaterialResource material;
            {
                material.name = filePath + "%material%" + mat->GetName().C_Str();
                material.renderMode = (albedoColor.w < 1.0f) ? RenderMode::Transparent : RenderMode::Opaque;

                material.baseColor = albedoColor;

                if (!albedoTexturePath.empty()) {
                    material.albedoTexture = TEXTURE_REPOSITORY->GetTextureResource(albedoTexturePath);
                }
            }

            // マテリアルセットアップ
            if (modelMesh.materialIndex < model->materialResources.size() && modelMesh.materialIndex >= 0) {
                model->materialResources[modelMesh.materialIndex] = MATERIAL_REPOSITORY->GenerateMaterial(material);
            }
        }
    }

    return model;
}

// モデルの解放
void ModelRepository::ReleaseModel(const std::string& filePath)
{
    ModelResource* model = GetModel(filePath);
    if (model)
    {
        // メッシュの解放
        for (ModelMesh& mesh : model->meshes)
        {
            mesh.vertexBuffer.Reset();
            mesh.indexBuffer.Reset();
        }
        aiReleaseImport(model->AiScene);
    }
}
