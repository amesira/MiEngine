//===================================================
// transparent_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//===================================================
#include "transparent_render_pass.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/render_view.h"

#include "Engine/Framework/Component/particle_system_component.h"
#include "Engine/Framework/Component/transform_component.h"

#include "Engine/engine_service_locator.h"
using namespace ShaderDefinitions;

#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

void TransparentRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // デフォルトテクスチャの取得
    m_defaultTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\white.bmp");

    // ParticleRenderer用の頂点バッファを生成
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

    // 透明オブジェクトの描画設定
    SetBlendState(BLENDSTATE_ALFA);
    SetDepthState(DEPTHSTATE_ENABLE);

    // パーティクル描画のためのシェーダーをバインド
    EngineServiceLocator::BindShader(ShaderBase::Particle);

    auto& particleSystems = particlePool->GetList();
    for (ParticleSystemComponent& particleSystem : particleSystems) {
        if (!particleSystem.GetOwner()->GetActive()) continue;
        if (!particleSystem.GetEnable()) continue;

        // αブレンドのみで描画
        if (particleSystem.Renderer().blendMode != ParticleSystemComponent::BlendMode::AlphaBlend) continue;

        DrawParticleSystem(particleSystem, view);
    }

    // 加算発光の描画設定
    SetBlendState(BLENDSTATE_ADD);

    for (ParticleSystemComponent& particleSystem : particleSystems) {
        if (!particleSystem.GetOwner()->GetActive()) continue;
        if (!particleSystem.GetEnable()) continue;

        // 加算合成で描画
        if (particleSystem.Renderer().blendMode != ParticleSystemComponent::BlendMode::Additive) continue;

        DrawParticleSystem(particleSystem, view);
    }


    SetBlendState(BLENDSTATE_NONE);
}

// パーティクルシステムの描画
void TransparentRenderPass::DrawParticleSystem(ParticleSystemComponent& particleSystem, const RenderView& view)
{
    // === 頂点バッファの設定 ===
    D3D11_MAPPED_SUBRESOURCE msr = {};
    m_pContext->Map(m_pParticleVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    ParticleVertex* vertices = static_cast<ParticleVertex*>(msr.pData);
    {
        vertices[0].position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
        vertices[1].position = XMFLOAT3(0.5f, -0.5f, 0.0f);
        vertices[2].position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
        vertices[3].position = XMFLOAT3(0.5f, 0.5f, 0.0f);

        vertices[0].texCoord = XMFLOAT2(0.0f, 0.0f);
        vertices[1].texCoord = XMFLOAT2(1.0f, 0.0f);
        vertices[2].texCoord = XMFLOAT2(0.0f, 1.0f);
        vertices[3].texCoord = XMFLOAT2(1.0f, 1.0f);
    }
    m_pContext->Unmap(m_pParticleVertexBuffer.Get(), 0);

    // === パーティクルのインスタンスデータを更新 ===
    msr = {};
    m_pContext->Map(m_pParticleInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    ParticleInstanceData* instanceData = static_cast<ParticleInstanceData*>(msr.pData);

    // ビルボード行列の計算
    XMMATRIX billboardRotation = XMMatrixIdentity();
    switch (particleSystem.Renderer().billboardMode) {
    case ParticleSystemComponent::BillboardMode::View: {
        XMMATRIX billboard = XMMatrixInverse(nullptr, view.viewMatrix);
        billboard.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        billboardRotation = XMMatrixRotationY(XM_PI) * billboard;
        break;
    }
    case ParticleSystemComponent::BillboardMode::Horizontal: {
        const XMMATRIX invView = XMMatrixInverse(nullptr, view.viewMatrix);

        XMFLOAT3 cameraForward = {};
        XMStoreFloat3(&cameraForward, invView.r[2]);
        cameraForward.y = 0.0f;

        const float lengthSq = cameraForward.x * cameraForward.x + cameraForward.z * cameraForward.z;
        if (lengthSq <= 0.0001f) {
            billboardRotation = XMMatrixIdentity();
            break;
        }

        const float yaw = atan2f(cameraForward.x, cameraForward.z);
        billboardRotation = XMMatrixRotationY(XM_PI) * XMMatrixRotationY(yaw);
        break;
    }
    }

    int instanceCount = 0;

    const auto& particles = particleSystem.Particles();
    for (const auto& particle : particles) {
        if (!particle.alive) continue;
        if (instanceCount >= ParticleSystemComponent::MAX_PARTICLES) break;

        // ワールド行列の作成
        XMMATRIX world = XMMatrixIdentity();
        {
            XMMATRIX translate = XMMatrixTranslation(
                particle.position.x,
                particle.position.y,
                particle.position.z);
            XMMATRIX scale = XMMatrixScaling(particle.size, particle.size, particle.size);
            world = scale * billboardRotation * translate;
        }

        // インスタンスデータの設定
        instanceData[instanceCount].world = world;
        instanceData[instanceCount].color = particle.color;
        instanceData[instanceCount].uvRect = particleSystem.Renderer().uvRect;

        instanceCount++;
    }

    // インスタンスデータの更新完了
    m_pContext->Unmap(m_pParticleInstanceBuffer.Get(), 0);

    // テクスチャをバインド
    TextureResource* texture = particleSystem.Renderer().textureResource ?
        particleSystem.Renderer().textureResource : m_defaultTexture;
    if (texture) {
        m_pContext->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());
    }

    // 頂点バッファのバインド
    UINT stride[2] = { sizeof(ParticleVertex), sizeof(ParticleInstanceData) };
    UINT offset[2] = { 0, 0 };
    ID3D11Buffer* buffers[2] = { m_pParticleVertexBuffer.Get(), m_pParticleInstanceBuffer.Get() };
    m_pContext->IASetVertexBuffers(0, 2, buffers, stride, offset);

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pContext->DrawInstanced(4, instanceCount, 0, 0);
}
