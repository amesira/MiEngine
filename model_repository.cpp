//===================================================
// model_repository.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//===================================================
#include "model_repository.h"

#include "sprite.h"
#include "shader.h"
#include <memory>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

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
    LoadModel(filePath);
    return m_modelCache[filePath].get();
}

//------------------------------------

// モデルの読み込み
void ModelRepository::LoadModel(const std::string& filePath)
{
    m_modelCache[filePath] = std::make_unique<ModelResource>();
    ModelResource* model = m_modelCache[filePath].get();
    model->filePath = filePath;

    const aiScene* scene = aiImportFile(filePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
    assert(scene);
    model->AiScene = scene;

    // メッシュの読み込み
    model->meshes.reserve(scene->mNumMeshes);
    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh* mesh = scene->mMeshes[m];
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        // ModelMesh構造体にデータを格納
        ModelMesh& modelMesh = model->meshes.emplace_back();
        modelMesh.name = mesh->mName.C_Str();
        modelMesh.numVertices = mesh->mNumVertices;
        modelMesh.numIndices = mesh->mNumFaces * 3;

        // 頂点バッファ生成
        {
            Vertex* vertex = new Vertex[mesh->mNumVertices];

            for (unsigned int v = 0; v < mesh->mNumVertices; v++)
            {
                vertex[v].position = XMFLOAT3(mesh->mVertices[v].x, -mesh->mVertices[v].z, mesh->mVertices[v].y);
                vertex[v].texCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
                vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);

                // マテリアル取得
                XMFLOAT4 baseColor = { 1.0f,1.0f,1.0f,1.0f };
                aiColor4D c;
                if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_BASE_COLOR, &c) ||
                    AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &c))
                {
                    baseColor = { c.r, c.g, c.b, c.a };
                }

                vertex[v].color = baseColor;
            }

            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = sizeof(Vertex) * mesh->mNumVertices;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            D3D11_SUBRESOURCE_DATA sd;
            ZeroMemory(&sd, sizeof(sd));
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

            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
            bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
            bd.CPUAccessFlags = 0;

            D3D11_SUBRESOURCE_DATA sd;
            ZeroMemory(&sd, sizeof(sd));
            sd.pSysMem = index;
            m_pDevice->CreateBuffer(&bd, &sd, &modelMesh.indexBuffer);

            delete[] index;
        }
    }

    // テクスチャの読み込み
    model->textures.reserve(scene->mNumTextures);
    for (unsigned int i = 0; i < scene->mNumTextures; i++)
    {
        aiTexture* aitexture = scene->mTextures[i];

        ID3D11ShaderResourceView* texture;
        TexMetadata metadata;
        ScratchImage image;
        LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
        CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metadata, &texture);
        assert(texture);

        ModelTexture& modelTexture = model->textures.emplace_back();
        modelTexture.name = aitexture->mFilename.C_Str();
        modelTexture.texture = texture;
    }
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
        // テクスチャの解放
        for (ModelTexture& texture : model->textures)
        {
            texture.texture.Reset();
        }

        aiReleaseImport(model->AiScene);

    }
}
