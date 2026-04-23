//===================================================
// decal_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/13
//===================================================
#include "decal_render_pass.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/decal_component.h"

#include "Engine/engine_service_locator.h"
#include "Utility/debug_renderer.h"
#include "Utility/debug_ostream.h"

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
        m_pContext->PSSetShaderResources(5, 1, &m_depthSRV);
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
           // XMMATRIX offset = XMMatrixTranslation(0.0f, 0.0f, d.GetProjectionDepth() / 2.0f);

            world = scaling * rotation * translation;
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

// デカール範囲のデバッグ描画
void DecalRenderPass::CollectDebugDraw(IScene* pScene)
{
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* decalPool = pScene->GetComponentPool<DecalComponent>();
    if (!transformPool || !decalPool)return;

    const DirectX::XMFLOAT4 debugColor = { 1.0f,1.0f,0.0f,1.0f };

    auto& decalPoolList = decalPool->GetList();
    for (DecalComponent& d : decalPoolList) {
        TransformComponent* t = transformPool->GetByGameObjectID(d.GetOwner()->GetID());
        
        // component無効チェック
        if (!t)continue;
        if (!d.GetEnable() || !t->GetEnable())continue;

        // 頂点座標を算出
        XMFLOAT3 halfScale = XMFLOAT3(d.GetProjectionSize().x, d.GetProjectionSize().y, d.GetProjectionDepth());
        //halfScale = MiMath::Multiply(halfScale, 0.5f);
        XMFLOAT3 verts[8] = {
            { -halfScale.x, -halfScale.y, -halfScale.z },
            {  halfScale.x, -halfScale.y, -halfScale.z },
            {  halfScale.x,  halfScale.y, -halfScale.z },
            { -halfScale.x,  halfScale.y, -halfScale.z },
            { -halfScale.x, -halfScale.y,  halfScale.z },
            {  halfScale.x, -halfScale.y,  halfScale.z },
            {  halfScale.x,  halfScale.y,  halfScale.z },
            { -halfScale.x,  halfScale.y,  halfScale.z },
        };

        //  回転させる
        for (int i = 0; i < 8; i++) {
            verts[i] = MiMath::RotateVector(t->GetRotation(), verts[i]);
            verts[i].x += t->GetPosition().x;
            verts[i].y += t->GetPosition().y;
            verts[i].z += t->GetPosition().z;
        }

        DebugRenderer_DrawLine(verts[0], verts[1], debugColor);
        DebugRenderer_DrawLine(verts[1], verts[2], debugColor);
        DebugRenderer_DrawLine(verts[2], verts[3], debugColor);
        DebugRenderer_DrawLine(verts[3], verts[0], debugColor);
        DebugRenderer_DrawLine(verts[4], verts[5], debugColor);
        DebugRenderer_DrawLine(verts[5], verts[6], debugColor);
        DebugRenderer_DrawLine(verts[6], verts[7], debugColor);
        DebugRenderer_DrawLine(verts[7], verts[4], debugColor);
        DebugRenderer_DrawLine(verts[0], verts[4], debugColor);
        DebugRenderer_DrawLine(verts[1], verts[5], debugColor);
        DebugRenderer_DrawLine(verts[2], verts[6], debugColor);
        DebugRenderer_DrawLine(verts[3], verts[7], debugColor);
    }
}

// -------------------------------- bind

// 深度テクスチャのセット
void DecalRenderPass::SetDepthTexture(ID3D11ShaderResourceView* depthSRV)
{
    m_depthSRV = depthSRV;
}

// 深度テクスチャのアンバインド
void DecalRenderPass::UnbindDepthTexture()
{
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_pContext->PSSetShaderResources(5, 1, nullSRV);

    m_depthSRV = nullptr;
}
