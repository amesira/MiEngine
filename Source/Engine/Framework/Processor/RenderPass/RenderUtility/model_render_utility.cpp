//===================================================
// model_render_utility.cpp
//===================================================
#include "model_render_utility.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/transform_component.h"

namespace ModelRenderUtility {

    DirectX::XMMATRIX CreateWorldMatrix(const TransformComponent& transform)
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

        return scaling * rotation * translation;
    }

    void DrawMeshGeometry(ID3D11DeviceContext* context, const ModelMesh& mesh)
    {
        if (!context) return;

        context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &mesh.vertexStride, &mesh.vertexOffset);
        context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(mesh.numIndices, 0, 0);
    }

    void DrawMeshListGeometry(ID3D11DeviceContext* context, const std::vector<ModelMesh>& meshes)
    {
        if (!context) return;

        for (const ModelMesh& mesh : meshes) {
            DrawMeshGeometry(context, mesh);
        }
    }

    void ForEachRenderableModel(
        IScene* scene,
        const std::function<void(ModelComponent& model, TransformComponent& transform, ModelResource& modelResource)>& callback)
    {
        if (!scene || !callback) return;

        auto* transformPool = scene->GetComponentPool<TransformComponent>();
        auto* modelPool = scene->GetComponentPool<ModelComponent>();
        if (!transformPool || !modelPool) return;

        auto& modelList = modelPool->GetList();
        for (ModelComponent& model : modelList) {
            TransformComponent* transform = transformPool->GetByGameObjectID(model.GetOwner()->GetID());
            if (!transform) continue;
            if (!model.GetEnable() || !transform->GetEnable()) continue;
            if (!model.GetModelResource()) continue;

            callback(model, *transform, *model.GetModelResource());
        }
    }

}
