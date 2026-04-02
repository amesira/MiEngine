//===================================================
// opaque_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//===================================================
#include "opaque_render_pass.h"
#include "scene_interface.h"
#include "game_object.h"

// directX
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
using namespace DirectX;

#include "transform_component.h"
#include "model_component.h"

#include "engine_service_locator.h"

#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()

// OpaqueRenderPassの初期化
void OpaqueRenderPass::Initialize()
{
    m_pDevice = Direct3D_GetDevice();
    m_pContext = Direct3D_GetDeviceContext();

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

    // 定数バッファ更新
    int shaderIndex = -1;

    // 描画ステートのセット
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_ENABLE);

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

        // ワールド行列計算
        XMMATRIX worldMatrix = XMMatrixIdentity();
        {
            XMMATRIX scaling = XMMatrixScaling(
                t->GetScaling().x,
                t->GetScaling().y,
                t->GetScaling().z);
            XMMATRIX rotation = XMMatrixRotationQuaternion(
                t->GetRotation());
            XMMATRIX translation = XMMatrixTranslation(
                t->GetPosition().x,
                t->GetPosition().y,
                t->GetPosition().z);

            worldMatrix = scaling * rotation * translation;
        }

        // シェーダーセット
        if (shaderIndex == -1) {
            shaderIndex = static_cast<int>(ShaderManager::ShaderType::Lit);
            EngineServiceLocator::BindShader(ShaderManager::ShaderType::Lit);
        }

        // 行列セット
        EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });
        
        // プリミティブトポロジ設定
        m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        for (unsigned int i = 0; i < model->meshes.size(); i++)
        {
            ModelMesh& mesh = model->meshes[i];
            MaterialInstance& mat = m.GetMaterialSlots()[mesh.materialIndex];

            // マテリアルバインド
            MaterialBufferData materialBufferData = mat.materialResource->CreateBufferData();
            materialBufferData.baseColor = mat.isOverrideBaseColor ? mat.overrideBaseColor : materialBufferData.baseColor;
            materialBufferData.emissiveColor = mat.isOverrideEmissiveColor ? mat.overrideEmissiveColor : materialBufferData.emissiveColor;
            MATERIAL_REPOSITORY->BindMaterialCB(materialBufferData);
            MATERIAL_REPOSITORY->BindMaterialTexture(*mat.materialResource);

            // 頂点バッファ設定
            UINT stride = sizeof(LitVertex);
            UINT offset = 0;
            m_pContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);

            // インデックスバッファ設定
            m_pContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

            // ポリゴン描画
            m_pContext->DrawIndexed(mesh.numIndices, 0, 0);
        }
    }
}

//---------------------------------------------
