//===================================================
// model_repository.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//===================================================
#include "model_repository.h"

#include <memory>
#include "mi_string.h"

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

    // スキニングCBの作成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(XMMATRIX) * 256;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(&bd, nullptr, m_skinningBuffer.GetAddressOf());
    auto shader = EngineServiceLocator::GetShaderManager();
    if (shader) {
        shader->RegisterCB(ShaderManager::ShaderType::SkinnedLit, 12, m_skinningBuffer.GetAddressOf());
    }
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

// スキニングCBのバインド
void ModelRepository::BindSkinningCB(const std::vector<XMMATRIX>& boneMatrix)
{
    D3D11_MAPPED_SUBRESOURCE msr = {};
    m_pContext->Map(m_skinningBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    XMMATRIX* skinningData = reinterpret_cast<XMMATRIX*>(msr.pData);
    for (size_t i = 0; i < boneMatrix.size(); i++)
    {
        XMMATRIX finalTransformTransposed = XMMatrixTranspose(boneMatrix[i]);
        skinningData[i] = finalTransformTransposed;
    }
    m_pContext->Unmap(m_skinningBuffer.Get(), 0);
}

//------------------------------------

// モデルの読み込み
ModelResource* ModelRepository::LoadModel(const std::string& filePath)
{
    const aiScene* scene = aiImportFile(
        filePath.c_str(), 
        aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate);

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
            modelBone.offsetMatrix = AssimpMatToXMMatrix(bone->mOffsetMatrix);

            model->bones.emplace_back(modelBone);
            model->boneNameToIndex[modelBone.name] = modelBone.index;
        }

        // 頂点バッファ生成
        bool isSkinnedMesh = mesh->mNumBones > 0;
        if (!isSkinnedMesh) {
            LitVertex* vertex = new LitVertex[mesh->mNumVertices];
            model->vertexType = ModelResource::VertexType::Lit;

            // 頂点データを構造体に格納
            SetLitVertexInfo(vertex, mesh);

            // 頂点バッファの作成
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
            model->vertexType = ModelResource::VertexType::SkinnedLit;

            // 頂点データを構造体に格納
            SetSkinnedLitVertexInfo(vertex, mesh, model->boneNameToIndex);

            // 頂点バッファの作成
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
            MaterialResource material = CreateMaterialResource(mat);
            material.name = filePath + "_mat%" + mat->GetName().C_Str();

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
    model->defaultPose.defaultPositions.resize(model->bones.size(), XMFLOAT4(0, 0, 0, 0));
    model->defaultPose.defaultRotations.resize(model->bones.size(), XMFLOAT4(0, 0, 0, 1));
    model->defaultPose.defaultScales.resize(model->bones.size(), XMFLOAT4(1, 1, 1, 0));
    model->defaultPose.localTransforms.resize(model->bones.size(), XMMatrixIdentity());
    model->defaultPose.globalTransforms.resize(model->bones.size(), XMMatrixIdentity());
    model->defaultPose.boneTransforms.resize(model->bones.size(), XMMatrixIdentity());

    std::function<void(aiNode*,unsigned int, const XMMATRIX&)> buildBoneHierarchy = [&](aiNode* node, unsigned int parentIndex, const XMMATRIX& parentTransform) {

        XMMATRIX localTransform = AssimpMatToXMMatrix(node->mTransformation);
        XMMATRIX globalTransform = localTransform * parentTransform;

        int boneIndex = parentIndex;

        // ノード名がボーン名と一致する場合、ボーンのグローバル変換行列を保存
        auto it = model->boneNameToIndex.find(node->mName.C_Str());
        if (it != model->boneNameToIndex.end()) {
            boneIndex = it->second;

            model->defaultPose.localTransforms[boneIndex] = localTransform;
            model->defaultPose.globalTransforms[boneIndex] = globalTransform;
            model->bones[boneIndex].parentIndex = parentIndex;

            // デフォルトの位置、回転、スケールを分解して保存
            aiVector3D scaling, translation;
            aiQuaternion rotation;
            node->mTransformation.Decompose(scaling, rotation, translation);
            model->defaultPose.defaultPositions[boneIndex] = XMFLOAT4(translation.x, translation.y, translation.z, 0.0f);
            model->defaultPose.defaultRotations[boneIndex] = XMFLOAT4(rotation.x, rotation.y, rotation.z, rotation.w);
            model->defaultPose.defaultScales[boneIndex] = XMFLOAT4(scaling.x, scaling.y, scaling.z, 0.0f);
        }

        // 子ノードを再帰的に処理
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            buildBoneHierarchy(node->mChildren[i], boneIndex, globalTransform);
        }
    };
    buildBoneHierarchy(scene->mRootNode, UINT_MAX, XMMatrixIdentity());

    // ボーンの子インデックスの構築
    for (unsigned int i = 0; i < model->bones.size(); i++) {
        unsigned int parentIndex = model->bones[i].parentIndex;
        if (parentIndex < model->bones.size()) {
            model->bones[parentIndex].childIndices.push_back(i);
        }
        else { 
            // ルートボーンの場合
            // memo: mRootNodeがRootBoneとは限らないため、親インデックスが存在しないボーンをルートとみなす
            model->rootBoneIndex = i;
            model->rootParentCorrection = XMMatrixInverse(nullptr, model->defaultPose.localTransforms[i]);
        }
    }

    // ボーン最終行列の計算
    for (int i = 0; i < model->bones.size(); i++) {
        model->defaultPose.boneTransforms[i] = model->bones[i].offsetMatrix * model->defaultPose.globalTransforms[i];
    }

    // アニメーションデータの読み込み
    for (int a = 0; a < scene->mNumAnimations; a++) {
        aiAnimation* anim = scene->mAnimations[a];

        // AnimationClip構造体にデータを格納
        AnimationClip& clip = model->animationClips.emplace_back();
        clip.name = anim->mName.C_Str();
        float tps = static_cast<float>(anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0); // デフォルトは25ティック/秒
        clip.duration = anim->mDuration / tps;
        clip.ticksPerSecond = tps;

        for (unsigned int c = 0; c < anim->mNumChannels; c++) {
            aiNodeAnim* channel = anim->mChannels[c];

            // AnimationChannel構造体にデータを格納
            AnimationClip::AnimationChannel& channelData = clip.channels.emplace_back();
            channelData.boneName = channel->mNodeName.C_Str();
            auto it = model->boneNameToIndex.find(channelData.boneName);
            if (it != model->boneNameToIndex.end()) {
                channelData.boneIndex = it->second;
            }
            else {
                channelData.boneIndex = UINT_MAX;
            }

            // キーフレームデータのコピー
            for (unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
                aiVectorKey& posKey = channel->mPositionKeys[k];
                channelData.positionKeyframes.emplace_back(posKey.mTime / tps, XMFLOAT4(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z, 0.0f));
            }
            for (unsigned int k = 0; k < channel->mNumRotationKeys; k++) {
                aiQuatKey& rotKey = channel->mRotationKeys[k];
                channelData.rotationKeyframes.emplace_back(rotKey.mTime / tps, XMFLOAT4(rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z, rotKey.mValue.w));
            }
            for (unsigned int k = 0; k < channel->mNumScalingKeys; k++) {
                aiVectorKey& scaleKey = channel->mScalingKeys[k];
                channelData.scalingKeyframes.emplace_back(scaleKey.mTime / tps, XMFLOAT4(scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z, 0.0f));
            }

            // キーフレームで昇順ソート
            std::sort(channelData.positionKeyframes.begin(), channelData.positionKeyframes.end(), [](const auto& a, const auto& b) { return a.time < b.time; });
            std::sort(channelData.rotationKeyframes.begin(), channelData.rotationKeyframes.end(), [](const auto& a, const auto& b) { return a.time < b.time; });
            std::sort(channelData.scalingKeyframes.begin(), channelData.scalingKeyframes.end(), [](const auto& a, const auto& b) { return a.time < b.time; });
        }
    }

    return model;
}

// Assimpの行列をXMMATRIXに変換
XMMATRIX ModelRepository::AssimpMatToXMMatrix(const aiMatrix4x4& m)
{
    XMMATRIX xmMat = XMMATRIX(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4);
    return xmMat;
}

// aiMeshから頂点バッファを作成
void ModelRepository::SetLitVertexInfo(LitVertex* vertices, const aiMesh* mesh)
{
    for (unsigned int v = 0; v < mesh->mNumVertices; v++)
    {
        vertices[v].position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
        vertices[v].texCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
        vertices[v].normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
        vertices[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    return;
}

// aiMeshからスキニング頂点バッファを作成
void ModelRepository::SetSkinnedLitVertexInfo(SkinnedLitVertex* vertices, const aiMesh* mesh, const std::unordered_map<std::string, unsigned int>& boneNameToIndex)
{
    for (unsigned int v = 0; v < mesh->mNumVertices; v++)
    {
        vertices[v].position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
        vertices[v].texCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
        vertices[v].normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
        vertices[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

        // ボーンの影響を一時バッファへ格納
        std::array<unsigned int, 4> boneIndices = { 0, 0, 0, 0 };
        std::array<float, 4> boneWeights = { 0.0f, 0.0f, 0.0f, 0.0f };

        for (unsigned int b = 0; b < mesh->mNumBones; b++) {
            for (unsigned int w = 0; w < mesh->mBones[b]->mNumWeights; w++) {
                if (mesh->mBones[b]->mWeights[w].mVertexId != v) continue;

                // インデックスと重みを取得
                unsigned int boneIndex = boneNameToIndex.at(mesh->mBones[b]->mName.C_Str());
                float weight = mesh->mBones[b]->mWeights[w].mWeight;
                
                // 自分より小さい重みを持つスロットを探し、そこにボーンインデックスと重みを挿入
                int arrayIndex = std::find_if(boneWeights.begin(), boneWeights.end(), [weight](float w) { return w < weight; }) - boneWeights.begin();
                if (arrayIndex < 4) {
                    boneIndices[arrayIndex] = boneIndex;
                    boneWeights[arrayIndex] = weight;
                }
                
            }
        }

        // 重みの正規化
        float totalWeight = 0.0f;
        for (int i = 0; i < 4; i++) {
            totalWeight += boneWeights[i];
        }

        for (int i = 0; i < 4; i++) {
            boneIndices[i] = boneIndices[i];
            boneWeights[i] = boneWeights[i] / totalWeight;

        }

        // スキニング情報を頂点構造体に格納
        vertices[v].boneIndices = XMUINT4(boneIndices[0], boneIndices[1], boneIndices[2], boneIndices[3]);
        vertices[v].boneWeights = XMFLOAT4(boneWeights[0], boneWeights[1], boneWeights[2], boneWeights[3]);
    }
}

// aiMaterialからMaterialResourceを作成
MaterialResource ModelRepository::CreateMaterialResource(aiMaterial* mat)
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
        material.renderMode = (albedoColor.w < 1.0f) ? RenderMode::Transparent : RenderMode::Opaque;

        material.baseColor = albedoColor;

        if (!albedoTexturePath.empty()) {
            material.albedoTexture = TEXTURE_REPOSITORY->GetTextureResource(albedoTexturePath);
        }
    }

    return material;
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
