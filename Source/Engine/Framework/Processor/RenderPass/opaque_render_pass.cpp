//===================================================
// opaque_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//===================================================
#include "opaque_render_pass.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/model_component.h"

#include "Engine/engine_service_locator.h"

#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()

// OpaqueRenderPassの初期化
void OpaqueRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_defaultTexture = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Texture\\white.bmp");
}

// OpaqueRenderPassの終了処理
void OpaqueRenderPass::Finalize()
{

}

// OpaqueRenderPassの処理
void OpaqueRenderPass::Process(IScene* pScene)
{
    // コンポーネントプール取得
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* modelPool = pScene->GetComponentPool<ModelComponent>();
    if (!transformPool || !modelPool)return;

    // 描画ステートのセット
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_ENABLE);

    // シェーダーの初期セット
    ModelResource::VertexType currentVertexType = ModelResource::VertexType::Lit;
    EngineServiceLocator::BindShader(ShaderManager::ShaderType::Lit);

    // モデル描画
    auto& modelPoolList = modelPool->GetList();
    for (ModelComponent& m : modelPoolList) {
        TransformComponent* t = transformPool->GetByGameObjectID(m.GetOwner()->GetID());

        // component無効チェック
        if (!t)continue;
        if (!m.GetEnable() || !t->GetEnable())continue;

        // モデルデータ取得
        ModelResource* model = m.GetModelResource();
        if (!model)continue;

        // シェーダー切替
        if (currentVertexType != model->vertexType) {
            switch (model->vertexType) {
            case ModelResource::VertexType::Lit:
                EngineServiceLocator::BindShader(ShaderManager::ShaderType::Lit);
                break;
            case ModelResource::VertexType::SkinnedLit:
                EngineServiceLocator::BindShader(ShaderManager::ShaderType::SkinnedLit);
                break;
            }
            currentVertexType = model->vertexType;
        }

        // ワールド行列計算
        XMMATRIX worldMatrix = XMMatrixIdentity();
        {
            XMMATRIX scaling = XMMatrixScaling(
                t->GetScaling().x,
                t->GetScaling().y,
                t->GetScaling().z);
            XMMATRIX rotation = XMMatrixRotationQuaternion(t->GetRotationVector());
            XMMATRIX translation = XMMatrixTranslation(
                t->GetPosition().x,
                t->GetPosition().y,
                t->GetPosition().z);

            worldMatrix = scaling * rotation * translation;
        }

        // Transformバッファをバインド
        EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

        // スキニングCBバインド
        if (currentVertexType == ModelResource::VertexType::SkinnedLit) {
            EngineServiceLocator::GetModelRepository()->BindSkinningCB(m.GetSkeletonPose().boneTransforms);
        }

        // プリミティブトポロジ設定
        m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        UINT stride = 0;
        UINT offset = 0;
        switch (model->vertexType) {
            case ModelResource::VertexType::Lit:
                stride = sizeof(LitVertex);
                break;
            case ModelResource::VertexType::SkinnedLit:
                stride = sizeof(SkinnedLitVertex);
                break;
        }

        for (unsigned int i = 0; i < model->meshes.size(); i++)
        {
            ModelMesh& mesh = model->meshes[i];
            MaterialInstance& mat = m.GetMaterialSlots()[mesh.materialIndex];
            if (!mat.materialResource)continue;
            if (mat.materialResource->renderMode != RenderMode::Opaque)continue;

            // マテリアルバインド
            MaterialBufferData materialBufferData = mat.materialResource->CreateBufferData();
            materialBufferData.baseColor = mat.isOverrideBaseColor ? mat.overrideBaseColor : materialBufferData.baseColor;
            materialBufferData.emissiveColor = mat.isOverrideEmissiveColor ? mat.overrideEmissiveColor : materialBufferData.emissiveColor;
            MATERIAL_REPOSITORY->BindMaterialCB(materialBufferData);
            MATERIAL_REPOSITORY->BindMaterialTexture(*mat.materialResource);

            // 頂点バッファ設定
            m_pContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);

            // インデックスバッファ設定
            m_pContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

            // ポリゴン描画
            m_pContext->DrawIndexed(mesh.numIndices, 0, 0);
        }
    }
}
