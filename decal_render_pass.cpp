//===================================================
// decal_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/13
//===================================================
#include "decal_render_pass.h"
#include "scene_interface.h"
#include "game_object.h"

#include "transform_component.h"
#include "decal_component.h"

#include "engine_service_locator.h"

// DecalRenderPassの初期化
void DecalRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // デカール描画用のモデルリソース取得
    m_decalCubeResource = EngineServiceLocator::GetModelRepository()->GetModel("asset\\Model\\cube.fbx");
}

// DecalRenderPassの終了処理
void DecalRenderPass::Finalize()
{

}

// DecalRenderPassの処理
void DecalRenderPass::Process(IScene* pScene)
{
    // コンポーネントプール取得
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* decalPool = pScene->GetComponentPool<DecalComponent>();
    if (!transformPool || !decalPool)return;

    // 描画ステートのセット
    SetBlendState(BLENDSTATE_ALFA);
    SetDepthState(DEPTHSTATE_ENABLE);

    // シェーダーの初期セット
    EngineServiceLocator::BindShader(ShaderManager::ShaderType::DecalLit);
    if (m_depthSRV) {
        m_pContext->PSSetShaderResources(5, 1, m_depthSRV.GetAddressOf());
    }

    // デカール描画
    auto& decalPoolList = decalPool->GetList();
    for (DecalComponent& d : decalPoolList) {
        TransformComponent* t = transformPool->GetByGameObjectID(d.GetOwner()->GetID());

        // component無効チェック
        if (!t)continue;
        if (!d.GetEnable() || !t->GetEnable())continue;

        // デカールテクスチャ取得
        TextureResource* decalTexture = d.GetDecalTexture();
        if (!decalTexture)continue;

        // ワールド行列計算
        XMMATRIX world = XMMatrixIdentity();
        {
            XMMATRIX translation = XMMatrixTranslation(t->GetPosition().x, t->GetPosition().y, t->GetPosition().z);
            XMMATRIX rotation = XMMatrixRotationQuaternion(t->GetRotationVector());
            XMMATRIX scaling = XMMatrixScaling(d.GetProjectionSize().x, d.GetProjectionSize().y, d.GetProjectionDepth());
            XMMATRIX offset = XMMatrixTranslation(0.0f, 0.0f, d.GetProjectionDepth() / 2.0f);

            world = offset * scaling * rotation * translation;
        }

        // ワールド行列をTransformCBにセット
        EngineServiceLocator::UpdateTransformCB({ world, XMMatrixIdentity()});

        // テクスチャセット
        m_pContext->PSSetShaderResources(0, 1, decalTexture->texture.GetAddressOf());

        // プリミティブトポロジ設定
        m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        if (m_decalCubeResource->meshes.empty()) continue;
        ModelMesh& mesh = m_decalCubeResource->meshes[0];
        {
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