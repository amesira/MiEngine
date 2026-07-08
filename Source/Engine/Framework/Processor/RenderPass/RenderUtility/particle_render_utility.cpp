//===================================================
// particle_render_utility.cpp
//===================================================
#include "particle_render_utility.h"

#include "Engine/Framework/Component/particle_system_component.h"
#include "Engine/Graphics/shader_definitions.h"
#include "Engine/render_view.h"

namespace ParticleRenderUtility {

    bool UpdateParticleQuadVertexBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* vertexBuffer)
    {
        if (!context || !vertexBuffer) return false;

        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        HRESULT hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return false;

        ShaderDefinitions::ParticleVertex* vertices =
            static_cast<ShaderDefinitions::ParticleVertex*>(mappedResource.pData);

        vertices[0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f);
        vertices[1].position = DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f);
        vertices[2].position = DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f);
        vertices[3].position = DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f);

        vertices[0].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
        vertices[1].texCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
        vertices[2].texCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
        vertices[3].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);

        context->Unmap(vertexBuffer, 0);
        return true;
    }

    DirectX::XMMATRIX CreateBillboardRotation(
        ParticleSystemData::BillboardMode billboardMode,
        const RenderView& view)
    {
        using namespace DirectX;

        switch (billboardMode) {
        case ParticleSystemData::BillboardMode::View: {
            XMMATRIX billboard = XMMatrixInverse(nullptr, view.viewMatrix);
            billboard.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
            return XMMatrixRotationY(XM_PI) * billboard;
        }
        case ParticleSystemData::BillboardMode::Horizontal: {
            const XMMATRIX invView = XMMatrixInverse(nullptr, view.viewMatrix);

            XMFLOAT3 cameraForward = {};
            XMStoreFloat3(&cameraForward, invView.r[2]);
            cameraForward.y = 0.0f;

            const float lengthSq = cameraForward.x * cameraForward.x + cameraForward.z * cameraForward.z;
            if (lengthSq <= 0.0001f) {
                return XMMatrixIdentity();
            }

            const float yaw = atan2f(cameraForward.x, cameraForward.z);
            return XMMatrixRotationY(XM_PI) * XMMatrixRotationY(yaw);
        }
        default:
            return XMMatrixIdentity();
        }
    }

    int UpdateParticleInstanceBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* instanceBuffer,
        const ParticleSystemComponent& particleSystem,
        const DirectX::XMMATRIX& billboardRotation)
    {
        if (!context || !instanceBuffer) return 0;

        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        HRESULT hr = context->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return 0;

        ShaderDefinitions::ParticleInstanceData* instanceData =
            static_cast<ShaderDefinitions::ParticleInstanceData*>(mappedResource.pData);

        int instanceCount = 0;
        const auto& particles = particleSystem.Particles();
        const auto& renderer = particleSystem.GetDesc().rendererModule;

        for (const auto& particle : particles) {
            if (!particle.alive) continue;
            if (instanceCount >= ParticleSystemComponent::MAX_PARTICLES) break;

            DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(
                particle.position.x,
                particle.position.y,
                particle.position.z);
            DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(
                particle.size,
                particle.size,
                particle.size);

            instanceData[instanceCount].world = scale * billboardRotation * translate;
            instanceData[instanceCount].color = particle.color;

            // UvRectの計算（掛け合わせる）
            DirectX::XMFLOAT4 uvRect = renderer.uvRect;
            uvRect.x += particle.uvRect.x * uvRect.z;
            uvRect.y += particle.uvRect.y * uvRect.w;
            uvRect.z *= particle.uvRect.z;
            uvRect.w *= particle.uvRect.w;
            instanceData[instanceCount].uvRect = uvRect;

            instanceCount++;
        }

        context->Unmap(instanceBuffer, 0);
        return instanceCount;
    }

}
