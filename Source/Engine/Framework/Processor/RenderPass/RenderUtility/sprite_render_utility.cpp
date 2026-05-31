//===================================================
// sprite_render_utility.cpp
//===================================================
#include "sprite_render_utility.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"
#include "Engine/Framework/Component/transform_component.h"

#include "Engine/Graphics/shader_definitions.h"

namespace SpriteRenderUtility {
    // ワールド行列の作成
    DirectX::XMMATRIX CreateWorldMatrix(const TransformComponent& transform, bool applyFlipRotation)
    {
        using namespace DirectX;

        const XMFLOAT3 scalingValue = transform.GetScaling();
        const XMFLOAT3 positionValue = transform.GetPosition();

        XMMATRIX scaling = XMMatrixScaling(
            scalingValue.x,
            scalingValue.y,
            scalingValue.z);
        XMMATRIX rotation = XMMatrixRotationQuaternion(transform.GetRotationVector());
        XMMATRIX translation = XMMatrixTranslation(
            positionValue.x,
            positionValue.y,
            positionValue.z);

        if (!applyFlipRotation) {
            return scaling * rotation * translation;
        }

        XMMATRIX flipRotation = XMMatrixRotationY(XM_PI);
        return scaling * flipRotation * rotation * translation;
    }
    // フリップの適用されたUV矩形を取得
    DirectX::XMFLOAT4 ApplyFlipToUvRect(
        DirectX::XMFLOAT4 uvRect,
        bool flipX,
        bool flipY)
    {
        if (flipX) {
            uvRect.x += uvRect.z;
            uvRect.z *= -1.0f;
        }
        if (flipY) {
            uvRect.y += uvRect.w;
            uvRect.w *= -1.0f;
        }

        return uvRect;
    }
    // スプライト頂点バッファをUV矩形と色で更新
    bool UpdateSpriteVertexBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* vertexBuffer,
        const DirectX::XMFLOAT4& uvRect,
        const DirectX::XMFLOAT4& color)
    {
        if (!context || !vertexBuffer) return false;

        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        HRESULT hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return false;

        constexpr float halfSize = 0.5f;

        ShaderDefinitions::SpriteVertex* vertices = static_cast<ShaderDefinitions::SpriteVertex*>(mappedResource.pData);

        vertices[0].position = DirectX::XMFLOAT3(-halfSize, -halfSize, 0.0f);
        vertices[1].position = DirectX::XMFLOAT3(halfSize, -halfSize, 0.0f);
        vertices[2].position = DirectX::XMFLOAT3(-halfSize, halfSize, 0.0f);
        vertices[3].position = DirectX::XMFLOAT3(halfSize, halfSize, 0.0f);

        vertices[0].texCoord = DirectX::XMFLOAT2(uvRect.x, uvRect.y + uvRect.w);
        vertices[1].texCoord = DirectX::XMFLOAT2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
        vertices[2].texCoord = DirectX::XMFLOAT2(uvRect.x, uvRect.y);
        vertices[3].texCoord = DirectX::XMFLOAT2(uvRect.x + uvRect.z, uvRect.y);

        for (int i = 0; i < 4; i++) {
            vertices[i].color = color;
            vertices[i].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
        }

        context->Unmap(vertexBuffer, 0);
        return true;
    }

    // シーン内の描画可能なスプライトに対してコールバックを実行
    void ForEachRenderableSprite(
        IScene* scene,
        const std::function<void(SpriteRendererComponent& sprite, TransformComponent& transform)>& callback)
    {
        if (!scene || !callback) return;

        auto* transformPool = scene->GetComponentPool<TransformComponent>();
        auto* spritePool = scene->GetComponentPool<SpriteRendererComponent>();
        if (!transformPool || !spritePool) return;

        auto& spriteList = spritePool->GetList();
        for (SpriteRendererComponent& sprite : spriteList) {
            TransformComponent* transform = transformPool->GetByGameObjectID(sprite.GetOwner()->GetID());
            if (!transform) continue;
            if (!sprite.GetEnable()) continue;

            callback(sprite, *transform);
        }
    }

}
