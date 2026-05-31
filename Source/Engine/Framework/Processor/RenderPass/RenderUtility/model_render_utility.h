//---------------------------------------------------
// model_render_utility.h
// ・モデル描画に関連する共通処理
//---------------------------------------------------
#ifndef MODEL_RENDER_UTILITY_H
#define MODEL_RENDER_UTILITY_H

#include <functional>
#include <vector>

#include "Engine/Device/direct3d.h"
#include "Engine/Graphics/model_resource.h"

class IScene;
class ModelComponent;
class TransformComponent;

namespace ModelRenderUtility {

    DirectX::XMMATRIX CreateWorldMatrix(const TransformComponent& transform);

    void DrawMeshGeometry(ID3D11DeviceContext* context, const ModelMesh& mesh);
    void DrawMeshListGeometry(ID3D11DeviceContext* context, const std::vector<ModelMesh>& meshes);

    void ForEachRenderableModel(
        IScene* scene,
        const std::function<void(ModelComponent& model, TransformComponent& transform, ModelResource& modelResource)>& callback);

}

#endif // MODEL_RENDER_UTILITY_H
