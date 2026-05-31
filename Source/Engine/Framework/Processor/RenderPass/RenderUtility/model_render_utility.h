//---------------------------------------------------
// model_render_utility.h
// ・モデルレンダリングに関連するユーティリティ関数の宣言
//---------------------------------------------------
#ifndef MODEL_RENDER_UTILITY_H
#define MODEL_RENDER_UTILITY_H

#include <functional>
#include <vector>

#include "Engine/Device/direct3d.h"
#include "Engine/Graphics/model_resource.h"
#include "Engine/Graphics/shader_definitions.h"

class IScene;
class ModelComponent;
class TransformComponent;

namespace ModelRenderUtility {
    // ワールド行列の作成
    DirectX::XMMATRIX CreateWorldMatrix(const TransformComponent& transform);

    // メッシュのジオメトリを描画
    void DrawMeshGeometry(ID3D11DeviceContext* context, const ModelMesh& mesh);
    // メッシュのリストのジオメトリを描画
    void DrawMeshListGeometry(ID3D11DeviceContext* context, const std::vector<ModelMesh>& meshes);

    // シーン内の描画可能なモデルに対してコールバックを実行
    void ForEachRenderableModel(
        IScene* scene,
        const std::function<void(ModelComponent& model, TransformComponent& transform, ModelResource& modelResource)>& callback);

}

#endif // MODEL_RENDER_UTILITY_H
