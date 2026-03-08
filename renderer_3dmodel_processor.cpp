//===================================================
// renderer_3dmodel_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/12/27
//===================================================
#include "renderer_3dmodel_processor.h"
#include "scene_interface.h"
#include "game_object.h"

// directX
#include "d3d11.h"
#include "DirectXMath.h"
#include "direct3d.h"
using namespace DirectX;

// graphics, devices
#include "shader.h"
#include "sprite.h"
#include "keyboard.h"

#include "model.h"

// component
#include "transform_component.h"
#include "model_component.h"

void Renderer3DModelProcessor::Initialize()
{
    
}

void Renderer3DModelProcessor::Finalize()
{
    
}

void Renderer3DModelProcessor::Process(IScene* pScene)
{
    DirectX::XMMATRIX viewMatrix = Direct3D_GetViewMatrix();
    DirectX::XMMATRIX projectionMatrix = Direct3D_GetProjectionMatrix();

    // コンポーネントプール取得
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* modelPool = pScene->GetComponentPool<ModelComponent>();
    if (!transformPool || !modelPool)return;

    auto& modelPoolList = modelPool->GetList(); 

    for(ModelComponent& m : modelPoolList) {
        TransformComponent* t = transformPool->GetByGameObjectID(m.GetOwner()->GetID());

        // component無効チェック
        if (!t)continue;
        if (!m.GetEnable() || !t->GetEnable())continue;

        // モデルデータ取得
        MODEL* model = m.GetModel();
        if (!model)continue;

        // ワールド行列計算
        DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(
            t->GetScaling().x,
            t->GetScaling().y,
            t->GetScaling().z);
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(
            t->GetRotation());
        DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(
            t->GetPosition().x,
            t->GetPosition().y,
            t->GetPosition().z);

        DirectX::XMMATRIX worldMatrix = scaling * rotation * translation;

        // 行列セット
        Shader_SetMatrix(worldMatrix * viewMatrix * projectionMatrix);
        Shader_SetWorldMatrix(worldMatrix);

        Shader_SetPixelOption(m.GetColor(), 0.0f);

        // モデル描画
        ModelDraw(model);
    }

    // オプションリセット
    Shader_SetPixelOption(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
}