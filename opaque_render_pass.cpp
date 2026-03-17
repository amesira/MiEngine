//===================================================
// opaque_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//===================================================
#include "opaque_render_pass.h"
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

#include "model.h"

#include "transform_component.h"
#include "model_component.h"


// OpaqueRenderPassの初期化
void OpaqueRenderPass::Initialize()
{

}

// OpaqueRenderPassの終了処理
void OpaqueRenderPass::Finalize()
{

}

// OpaqueRenderPassの処理
void OpaqueRenderPass::Process(IScene* pScene)
{
    // コンポーネントプール取得
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* modelPool = pScene->GetComponentPool<ModelComponent>();
    if (!transformPool || !modelPool)return;

    auto& modelPoolList = modelPool->GetList();

    for (ModelComponent& m : modelPoolList) {
        TransformComponent* t = transformPool->GetByGameObjectID(m.GetOwner()->GetID());

        // component無効チェック
        if (!t)continue;
        if (!m.GetEnable() || !t->GetEnable())continue;

        // モデルデータ取得
        MODEL* model = m.GetModel();
        if (!model)continue;

        // ワールド行列計算
        XMMATRIX scaling = XMMatrixScaling(
            t->GetScaling().x,
            t->GetScaling().y,
            t->GetScaling().z);
        XMMATRIX rotation = XMMatrixRotationQuaternion(
            t->GetRotation());
        XMMATRIX translation = XMMatrixTranslation(
            t->GetPosition().x,
            t->GetPosition().y,
            t->GetPosition().z);

        XMMATRIX worldMatrix = scaling * rotation * translation;

        // 行列セット
        Shader_SetMatrix(m_viewMatrix * m_projectionMatrix);
        SetWorldMatrix(worldMatrix);

        Shader_SetPixelOption(m.GetColor(), 0.0f);

        // モデル描画
        ModelDraw(model);
    }

    // オプションリセット
    Shader_SetPixelOption(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
}
