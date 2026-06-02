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
#include "Engine/Framework/Component/transform_component.h"
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
}

void TransparentRenderPass::Finalize()
{
}

void TransparentRenderPass::Process(IScene* pScene, const RenderView& view)
{
    if (!pScene) return;

    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* particlePool = pScene->GetComponentPool<ParticleSystemComponent>();
    if (!transformPool || !particlePool) return;

    // アルファブレンドのパーティクルを描画
    SetBlendState(BLENDSTATE_ALFA);
    SetDepthState(DEPTHSTATE_ENABLE);

    EngineServiceLocator::BindShader(ShaderBase::Particle);

    auto& particleSystems = particlePool->GetList();
    for (ParticleSystemComponent& particleSystem : particleSystems) {
        if (!particleSystem.GetOwner()->GetActive()) continue;
        if (!particleSystem.GetEnable()) continue;
        if (particleSystem.Renderer().blendMode != ParticleSystemComponent::BlendMode::AlphaBlend) continue;

        DrawParticleSystem(particleSystem, view);
    }

    // 加算合成のパーティクルを描画
    SetBlendState(BLENDSTATE_ADD);

    for (ParticleSystemComponent& particleSystem : particleSystems) {
        if (!particleSystem.GetOwner()->GetActive()) continue;
        if (!particleSystem.GetEnable()) continue;
        if (particleSystem.Renderer().blendMode != ParticleSystemComponent::BlendMode::Additive) continue;

        DrawParticleSystem(particleSystem, view);
    }

    SetBlendState(BLENDSTATE_NONE);
}

// パーティクルシステムの描画
void TransparentRenderPass::DrawParticleSystem(ParticleSystemComponent& particleSystem, const RenderView& view)
{
    // 頂点バッファの更新
    if (!ParticleRenderUtility::UpdateParticleQuadVertexBuffer(m_pContext, m_pParticleVertexBuffer.Get())) return;

    // ビルボード行列の計算
    XMMATRIX billboardRotation = ParticleRenderUtility::CreateBillboardRotation(
        particleSystem.Renderer().billboardMode,
        view);

    // インスタンスバッファの更新
    int instanceCount = ParticleRenderUtility::UpdateParticleInstanceBuffer(
        m_pContext,
        m_pParticleInstanceBuffer.Get(),
        particleSystem,
        billboardRotation);
    if (instanceCount <= 0) return;

    // テクスチャの設定
    TextureResource* texture = particleSystem.Renderer().textureResource ?
        particleSystem.Renderer().textureResource : m_defaultTexture;
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
