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

#include <iostream>
#include <algorithm>

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
    if (scene == nullptr) return nullptr;

    // モデルリソースを作成してキャッシュに追加
    m_modelCache[filePath] = std::make_unique<ModelResource>();
    ModelResource* model = m_modelCache[filePath].get();

    // モデルリソースにシーンデータを格納
    model->name = filePath;
    model->AiScene = scene;
    model->meshes.reserve(scene->mNumMeshes);
    model->materialResources.resize(scene->mNumMaterials);
    model->bones.clear();
    model->boneNameToIndex.clear();

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

        // ボーンの読み込み
        for (unsigned int b = 0; b < mesh->mNumBones; b++) {
            aiBone* bone = mesh->mBones[b];

            // 同一名のボーンが既に存在するか確認
            if (model->boneNameToIndex.find(bone->mName.C_Str()) != model->boneNameToIndex.end())continue;

            // ModelBone構造体を追加し、データを格納
            ModelBone modelBone = {};
            modelBone.name = bone->mName.C_Str();
            modelBone.index = static_cast<unsigned int>(model->bones.size());
            modelBone.offsetMatrix = XMMATRIX(
                bone->mOffsetMatrix.a1, bone->mOffsetMatrix.a2, bone->mOffsetMatrix.a3, bone->mOffsetMatrix.a4,
                bone->mOffsetMatrix.b1, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.b4,
                bone->mOffsetMatrix.c1, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.c4,
                bone->mOffsetMatrix.d1, bone->mOffsetMatrix.d2, bone->mOffsetMatrix.d3, bone->mOffsetMatrix.d4
            );

            model->bones.emplace_back(modelBone);
            model->boneNameToIndex[modelBone.name] = modelBone.index;
        }

        // 頂点バッファ生成
        bool isSkinnedMesh = mesh->mNumBones > 0;
        if (!isSkinnedMesh) {
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
        else {
            SkinnedLitVertex* vertex = new SkinnedLitVertex[mesh->mNumVertices];

            for (unsigned int v = 0; v < mesh->mNumVertices; v++)
            {
                vertex[v].position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
                vertex[v].texCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
                vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
                vertex[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

                // ボーンの影響を一時バッファへ格納
                std::vector<std::pair<unsigned int, float>> boneWeightPairs;

                for (unsigned int b = 0; b < mesh->mNumBones; b++) {
                    for (unsigned int w = 0; w < mesh->mBones[b]->mNumWeights; w++) {
                        unsigned int boneIndex = model->boneNameToIndex[mesh->mBones[b]->mName.C_Str()];

                        if (mesh->mBones[b]->mWeights[w].mVertexId == v) {
                            ModelBone& bone = model->bones[boneIndex];
                            boneWeightPairs.emplace_back(
                                bone.index, 
                                mesh->mBones[b]->mWeights[w].mWeight);
                        }
                    }
                }

                // ボーンの影響を重みの大きい順にソート
                auto weightSort = [](const std::pair<unsigned int, float>& a, const std::pair<unsigned int, float>& b) {
                    return a.second > b.second;
                    };
                std::sort(boneWeightPairs.begin(), boneWeightPairs.end(), weightSort);

                // 上位4つのボーンのみを使用
                unsigned int boneIndices[4] = { 0, 0, 0, 0 };
                float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

                float totalWeight = 0.0f;
                for (int i = 0; i < 4; i++) {
                    if (i < (int)boneWeightPairs.size()) {
                        totalWeight += boneWeightPairs[i].second;
                    }
                }

                for (int i = 0; i < 4; i++) {
                    if (i < (int)boneWeightPairs.size()) {
                        boneIndices[i] = boneWeightPairs[i].first;
                        boneWeights[i] = boneWeightPairs[i].second / totalWeight; // 重みを正規化
                    }
                    else {
                        boneIndices[i] = 0;
                        boneWeights[i] = 0.0f;
                    }
                }

                vertex[v].boneIndices = XMINT4(boneIndices[0], boneIndices[1], boneIndices[2], boneIndices[3]);
                vertex[v].boneWeights = XMFLOAT4(boneWeights[0], boneWeights[1], boneWeights[2], boneWeights[3]);
            }

            D3D11_BUFFER_DESC bd = {};
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = sizeof(SkinnedLitVertex) * mesh->mNumVertices;
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

                // マテリアルリソースが未生成なら生成する
                if (model->materialResources[modelMesh.materialIndex] == nullptr) {
                    model->materialResources[modelMesh.materialIndex] = MATERIAL_REPOSITORY->GenerateMaterial(material);
                }
            }
        }
    }

    // ボーンの親子関係の構築
    std::function<void(aiNode*, const XMMATRIX&)> buildBoneHierarchy = [&](aiNode* node, const XMMATRIX& parentTransform) {

        XMMATRIX localTransform = XMMATRIX(
            node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
            node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
            node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
            node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4
        );
        XMMATRIX globalTransform = localTransform * parentTransform;

        // ノード名がボーン名と一致する場合、ボーンのグローバル変換行列を保存
        auto it = model->boneNameToIndex.find(node->mName.C_Str());
        if (it != model->boneNameToIndex.end()) {
            int boneIndex = it->second;
            model->bones[boneIndex].globalTransform = globalTransform;
        }

        // 子ノードを再帰的に処理
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            buildBoneHierarchy(node->mChildren[i], globalTransform);
        }
    };
    buildBoneHierarchy(scene->mRootNode, XMMatrixIdentity());

    // ボーン最終行列の計算
    for (ModelBone& bone : model->bones) {
        if (bone.index >= 0) {
            bone.finalTransform = bone.offsetMatrix * bone.globalTransform;
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
