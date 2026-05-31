//---------------------------------------------------
// sprite_render_utility.h
// ・スプライトレンダリングに関連するユーティリティ関数の宣言
//---------------------------------------------------
#ifndef SPRITE_RENDER_UTILITY_H
#define SPRITE_RENDER_UTILITY_H

#include <functional>

#include "Engine/Device/direct3d.h"
#include "Engine/Graphics/shader_definitions.h"
#include "Engine/Graphics/texture_resource.h"

class IScene;
class SpriteRendererComponent;
class TransformComponent;

namespace SpriteRenderUtility {
    // ワールド行列の作成
    DirectX::XMMATRIX CreateWorldMatrix(const TransformComponent& transform, bool applyFlipRotation = true);

    // フリップの適用されたUV矩形を取得
    DirectX::XMFLOAT4 ApplyFlipToUvRect(
        DirectX::XMFLOAT4 uvRect,
        bool flipX,
        bool flipY);

    // スプライト頂点バッファをUV矩形と色で更新
    bool UpdateSpriteVertexBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* vertexBuffer,
        const DirectX::XMFLOAT4& uvRect,
        const DirectX::XMFLOAT4& color);

    // シーン内の描画可能なスプライトに対してコールバックを実行
    void ForEachRenderableSprite(
        IScene* scene,
        const std::function<void(SpriteRendererComponent& sprite, TransformComponent& transform)>& callback);

}

#endif // SPRITE_RENDER_UTILITY_H
