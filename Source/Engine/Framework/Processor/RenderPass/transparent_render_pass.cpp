//===================================================
// transparent_render_pass.cpp
//
// Author: Miu Kitamura
// Date  : 2026/06/02
//===================================================
#include "transparent_render_pass.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/render_view.h"

#include "Engine/Framework/Component/particle_system_component.h"
#include "Engine/Framework/Component/line_renderer_component.h"
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Processor/RenderPass/RenderUtility/line_render_utility.h"
#include "Engine/Framework/Processor/RenderPass/RenderUtility/particle_render_utility.h"

#include "Engine/engine_service_locator.h"

using namespace ShaderDefinitions;

#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

void TransparentRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_defaultTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\white.bmp");

    {
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(ShaderDefinitions::ParticleVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer(&bd, NULL, m_pParticleVertexBuffer.GetAddressOf());
    }
    {
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(ShaderDefinitions::ParticleInstanceData) * ParticleSystemComponent::MAX_PARTICLES;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer(&bd, NULL, m_pParticleInstanceBuffer.GetAddressOf());
    }
    {
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(ShaderDefinitions::ParticleVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer(&bd, NULL, m_pLineVertexBuffer.GetAddressOf());
    }
    {
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(ShaderDefinitions::ParticleInstanceData) * LineRendererComponent::MAX_LINE_POINTS;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer(&bd, NULL, m_pLineInstanceBuffer.GetAddressOf());
    }
}

void TransparentRenderPass::Finalize()
{
}

void TransparentRenderPass::Process(IScene* pScene, const RenderView& view)
{
    if (!pScene) return;

    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* particlePool = pScene->GetComponentPool<ParticleSystemComponent>();
    auto* linePool = pScene->GetComponentPool<LineRendererComponent>();
    if (!transformPool && !particlePool && !linePool) return;

    // アルファブレンドのパーティクルを描画
    SetBlendState(BLENDSTATE_ALFA);
    SetDepthState(DEPTHSTATE_ENABLE);

    EngineServiceLocator::BindShader(ShaderBase::Particle);

    if (particlePool) {
        auto& particleSystems = particlePool->GetList();
        for (ParticleSystemComponent& particleSystem : particleSystems) {
            if (!particleSystem.GetOwner()->GetActive()) continue;
            if (!particleSystem.GetEnable()) continue;
            if (particleSystem.GetDesc().rendererModule.blendMode != ParticleSystemData::BlendMode::AlphaBlend) continue;

            DrawParticleSystem(particleSystem, view);
        }
    }

    if (linePool) {
        auto& lineRenderers = linePool->GetList();
        for (LineRendererComponent& lineRenderer : lineRenderers) {
            if (!lineRenderer.GetOwner()->GetActive()) continue;
            if (!lineRenderer.GetEnable()) continue;

            DrawLineRenderer(lineRenderer, view);
        }
    }

    // 加算合成のパーティクルを描画
    SetBlendState(BLENDSTATE_ADD);

    if (particlePool) {
        auto& particleSystems = particlePool->GetList();
        for (ParticleSystemComponent& particleSystem : particleSystems) {
            if (!particleSystem.GetOwner()->GetActive()) continue;
            if (!particleSystem.GetEnable()) continue;
            if (particleSystem.GetDesc().rendererModule.blendMode != ParticleSystemData::BlendMode::Additive) continue;

            DrawParticleSystem(particleSystem, view);
        }
    }

    SetBlendState(BLENDSTATE_NONE);
}

// パーティクルシステムの描画
void TransparentRenderPass::DrawParticleSystem(ParticleSystemComponent& particleSystem, const RenderView& view)
{
    // 頂点バッファの更新
    if (!ParticleRenderUtility::UpdateParticleQuadVertexBuffer(m_pContext, m_pParticleVertexBuffer.Get())) return;

    // ビルボード行列の計算
    auto& renderer = particleSystem.GetDesc().rendererModule;
    XMMATRIX billboardRotation = ParticleRenderUtility::CreateBillboardRotation(
        renderer.billboardMode,
        view);

    // インスタンスバッファの更新
    int instanceCount = ParticleRenderUtility::UpdateParticleInstanceBuffer(
        m_pContext,
        m_pParticleInstanceBuffer.Get(),
        particleSystem,
        billboardRotation);
    if (instanceCount <= 0) return;

    // テクスチャの設定
    TextureResource* texture = particleSystem.GetTextureResource() ? particleSystem.GetTextureResource() : m_defaultTexture;
    if (texture) {
        m_pContext->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());
    }

    UINT stride[2] = { sizeof(ParticleVertex), sizeof(ParticleInstanceData) };
    UINT offset[2] = { 0, 0 };
    ID3D11Buffer* buffers[2] = { m_pParticleVertexBuffer.Get(), m_pParticleInstanceBuffer.Get() };
    m_pContext->IASetVertexBuffers(0, 2, buffers, stride, offset);

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pContext->DrawInstanced(4, instanceCount, 0, 0);
}

// ラインの描画
void TransparentRenderPass::DrawLineRenderer(LineRendererComponent& lineRenderer, const RenderView& view)
{
    if (!LineRenderUtility::UpdateLineQuadVertexBuffer(m_pContext, m_pLineVertexBuffer.Get())) return;

    int instanceCount = LineRenderUtility::UpdateLineInstanceBuffer(
        m_pContext,
        m_pLineInstanceBuffer.Get(),
        lineRenderer,
        view,
        LineRendererComponent::MAX_LINE_POINTS);
    if (instanceCount <= 0) return;

    if (m_defaultTexture) {
        m_pContext->PSSetShaderResources(0, 1, m_defaultTexture->texture.GetAddressOf());
    }

    UINT stride[2] = { sizeof(ParticleVertex), sizeof(ParticleInstanceData) };
    UINT offset[2] = { 0, 0 };
    ID3D11Buffer* buffers[2] = { m_pLineVertexBuffer.Get(), m_pLineInstanceBuffer.Get() };
    m_pContext->IASetVertexBuffers(0, 2, buffers, stride, offset);

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pContext->DrawInstanced(4, instanceCount, 0, 0);
}
