//===================================================
// mask_render_pass.cpp
//
// Author: Miu Kitamura
// Date  : 2026/06/03
//===================================================
#include "mask_render_pass.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/render_view.h"

#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Processor/RenderPass/RenderUtility/model_render_utility.h"
#include "Engine/Framework/Processor/RenderPass/RenderUtility/sprite_render_utility.h"

#include "Engine/engine_service_locator.h"
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

namespace {
    bool IsLayerVisible(RenderLayer layer, RenderLayerMask mask)
    {
        return (mask & RenderLayerToMask(layer)) != 0;
    }
}

void MaskRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_defaultTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\white.bmp");

    // スプライト描画用の頂点バッファを作成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(ShaderDefinitions::SpriteVertex) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(&bd, NULL, &m_pSpriteVertexBuffer);
}

void MaskRenderPass::Finalize()
{
    if (m_pSpriteVertexBuffer) {
        m_pSpriteVertexBuffer->Release();
        m_pSpriteVertexBuffer = nullptr;
    }
}

void MaskRenderPass::Process(IScene* pScene, const RenderView& view)
{
    if (!pScene || !view.maskColorBufferRTV) return;

    // ビューポートのリセットとマスク用レンダーターゲットへの切り替え
    Direct3D_ResetViewport();
    Direct3D_ClearSceneTarget(view.maskColorBufferRTV.Get(), nullptr, 0.0f);
    Direct3D_SetSceneTarget(view.maskColorBufferRTV.Get(), nullptr);

    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    if (view.maskCullingMask == 0) return;

    // === モデルの描画 ===
    EngineServiceLocator::BindShader(ShaderBase::Unlit);
    if (m_defaultTexture) {
        m_pContext->PSSetShaderResources(0, 1, m_defaultTexture->texture.GetAddressOf());
    }

    ModelRenderUtility::ForEachRenderableModel(
        pScene,
        [this, &view](ModelComponent& m, TransformComponent& t, ModelResource& model)
        {
            GameObject* owner = m.GetOwner();
            if (!owner || !owner->GetActive()) return;
            if (!IsLayerVisible(owner->GetRenderLayer(), view.maskCullingMask)) return;

            XMMATRIX worldMatrix = ModelRenderUtility::CreateWorldMatrix(t);
            EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            DrawMeshList(model.meshes);
        });

    // === スプライトの描画 ===
    EngineServiceLocator::BindShader(ShaderBase::SpriteUnlit);

    SpriteRenderUtility::ForEachRenderableSprite(
        pScene,
        [this, &view](SpriteRendererComponent& s, TransformComponent& t)
        {
            GameObject* owner = s.GetOwner();
            if (!owner || !owner->GetActive()) return;
            if (!IsLayerVisible(owner->GetRenderLayer(), view.maskCullingMask)) return;

            XMMATRIX worldMatrix = SpriteRenderUtility::CreateWorldMatrix(t);
            EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

            TextureResource* texture = s.GetTextureResource() ? s.GetTextureResource() : m_defaultTexture;
            if (texture) {
                m_pContext->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());
            }

            XMFLOAT4 uvRect = SpriteRenderUtility::ApplyFlipToUvRect(s.GetUvRect(), s.GetFlipX(), s.GetFlipY());
            if (!SpriteRenderUtility::UpdateSpriteVertexBuffer(m_pContext, m_pSpriteVertexBuffer, uvRect, XMFLOAT4(1, 1, 1, 1))) {
                return;
            }

            UINT stride = sizeof(ShaderDefinitions::SpriteVertex);
            UINT offset = 0;
            m_pContext->IASetVertexBuffers(0, 1, &m_pSpriteVertexBuffer, &stride, &offset);
            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            m_pContext->Draw(4, 0);
        });
}

// メッシュの描画
void MaskRenderPass::DrawMeshList(const std::vector<ModelMesh>& meshes)
{
    for (const ModelMesh& mesh : meshes) {
        ModelRenderUtility::DrawMeshGeometry(m_pContext, mesh);
    }
}
