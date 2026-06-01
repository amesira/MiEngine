//===================================================
// transparent_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//===================================================
#include "transparent_render_pass.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"

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
        m_pDevice->CreateBuffer(&bd, NULL, &m_pParticleVertexBuffer);
    }
    {
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(ShaderDefinitions::ParticleInstanceData) * ParticleSystemComponent::MAX_PARTICLES;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer(&bd, NULL, &m_pParticleInstanceBuffer);
    }

}

void TransparentRenderPass::Finalize()
{
}

void TransparentRenderPass::Process(IScene* pScene)
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
        TransformComponent* transform = transformPool->GetByGameObjectID(particleSystem.GetOwner()->GetID());
        if (!transform) continue;
        if (!particleSystem.GetOwner()->GetActive()) continue;
        if (!particleSystem.GetEnable() || !transform->GetEnable()) continue;

        // 頂点バッファの設定
        D3D11_MAPPED_SUBRESOURCE msr = {};
        m_pContext->Map(m_pParticleVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
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
        m_pContext->Unmap(m_pParticleVertexBuffer, 0);

        // パーティクルのインスタンスデータを更新
        msr = {};
        m_pContext->Map(m_pParticleInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
        ParticleInstanceData* instanceData = static_cast<ParticleInstanceData*>(msr.pData);

        int instanceCount = 0;

        const auto& particles = particleSystem.Particles();
        for (const auto& particle : particles) {
            if (!particle.alive) continue;

            // ワールド行列の作成
            XMMATRIX world = XMMatrixIdentity();
            {
                XMMATRIX translate = XMMatrixTranslation(particle.position.x, particle.position.y, particle.position.z);
                XMMATRIX scale = XMMatrixScaling(particle.size, particle.size, particle.size);
                world = scale * translate;
            }

            // インスタンスデータの設定
            instanceData[instanceCount].worldRow0 = XMFLOAT4(world.r[0].m128_f32[0], world.r[0].m128_f32[1], 
                world.r[0].m128_f32[2], world.r[0].m128_f32[3]);
            instanceData[instanceCount].worldRow1 = XMFLOAT4(world.r[1].m128_f32[0], world.r[1].m128_f32[1], 
                world.r[1].m128_f32[2], world.r[1].m128_f32[3]);
            instanceData[instanceCount].worldRow2 = XMFLOAT4(world.r[2].m128_f32[0], world.r[2].m128_f32[1], 
                world.r[2].m128_f32[2], world.r[2].m128_f32[3]);
            instanceData[instanceCount].worldRow3 = XMFLOAT4(world.r[3].m128_f32[0], world.r[3].m128_f32[1], 
                world.r[3].m128_f32[2], world.r[3].m128_f32[3]);

            instanceData[instanceCount].color = particle.color;
            instanceData[instanceCount].uvRect = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f); // 仮

            instanceCount++;
        }

        // インスタンスデータの更新完了
        m_pContext->Unmap(m_pParticleInstanceBuffer, 0);

        // テクスチャをバインド
        TextureResource* texture = particleSystem.Renderer().textureResource ?
            particleSystem.Renderer().textureResource : m_defaultTexture;
        if (texture) {
            m_pContext->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());
        }

        // 頂点バッファのバインド
        UINT stride[2] = { sizeof(ParticleVertex), sizeof(ParticleInstanceData) };
        UINT offset[2] = { 0, 0 };
        ID3D11Buffer* buffers[2] = { m_pParticleVertexBuffer, m_pParticleInstanceBuffer };
        m_pContext->IASetVertexBuffers(0, 2, buffers, stride, offset);

        m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        m_pContext->DrawInstanced(4, instanceCount, 0, 0);
    }

    // 加算発光の描画設定
    //SetBlendState(BLENDSTATE_ADD);
}
