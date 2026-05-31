//---------------------------------------------------
// sprite_render_utility.h
// ・スプライト描画に関連する共通処理
//---------------------------------------------------
#ifndef SPRITE_RENDER_UTILITY_H
#define SPRITE_RENDER_UTILITY_H

#include <functional>

#include "Engine/Device/direct3d.h"

class IScene;
class SpriteRendererComponent;
class TransformComponent;

namespace SpriteRenderUtility {

    DirectX::XMMATRIX CreateWorldMatrix(const TransformComponent& transform, bool applyFlipRotation = true);

    DirectX::XMFLOAT4 ApplyFlipToUvRect(
        DirectX::XMFLOAT4 uvRect,
        bool flipX,
        bool flipY);

    bool UpdateSpriteVertexBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* vertexBuffer,
        const DirectX::XMFLOAT4& uvRect,
        const DirectX::XMFLOAT4& color);

    void ForEachRenderableSprite(
        IScene* scene,
        const std::function<void(SpriteRendererComponent& sprite, TransformComponent& transform)>& callback);

}

#endif // SPRITE_RENDER_UTILITY_H
