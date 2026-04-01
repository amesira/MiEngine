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

#include "transform_component.h"
#include "model_component.h"

#include "engine_service_locator.h"

static ID3D11ShaderResourceView* defaultTextureSRV = nullptr;

// OpaqueRenderPassの初期化
void OpaqueRenderPass::Initialize()
{
    m_pDevice = Direct3D_GetDevice();
    m_pContext = Direct3D_GetDeviceContext();

    auto resource = EngineServiceLocator::GetTextureRepository()->GetTexture("asset\\texture\\white.bmp");
    if (resource) {
        defaultTextureSRV = resource->texture.Get();
    }
    //LoadTexture(&defaultTextureSRV, L"asset\\texture\\white.bmp");
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

    EngineServiceLocator::BindShader(ShaderManager::ShaderType::Default);

    for (ModelComponent& m : modelPoolList) {
        TransformComponent* t = transformPool->GetByGameObjectID(m.GetOwner()->GetID());

        // component無効チェック
        if (!t)continue;
        if (!m.GetEnable() || !t->GetEnable())continue;

        // モデルデータ取得
        ModelResource* model = m.GetModelResource();
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
        /*Shader_SetMatrix(m_viewMatrix * m_projectionMatrix);
        SetWorldMatrix(worldMatrix);*/
        EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });
        EngineServiceLocator::UpdateCameraCB({
            m_viewMatrix,
            m_projectionMatrix,
            XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f)
            });

        //Shader_SetPixelOption(m.GetColor(), 0.0f);

        // モデル描画
        DrawModel(model, worldMatrix, m.GetColor());
    }

    // オプションリセット
    //Shader_SetPixelOption(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
}

//---------------------------------------------

// モデル描画
void OpaqueRenderPass::DrawModel(ModelResource* model, const XMMATRIX& world, const XMFLOAT4& color)
{
    // プリミティブトポロジ設定
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    model->meshes[0].numVertices;

    for (unsigned int m = 0; m < model->meshes.size(); m++)
    {
        ModelMesh& mesh = model->meshes[m];

        // テクスチャ設定の仮
        if (model->textures.empty()) {

        }
        m_pContext->PSSetShaderResources(0, 1, &defaultTextureSRV);

        //m_pContext->PSSetShaderResources(0, 1, model->textures[0].texture);

        // 頂点バッファ設定
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        m_pContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
        
        // インデックスバッファ設定
        m_pContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        
        // ポリゴン描画
        m_pContext->DrawIndexed(mesh.numIndices, 0, 0);
    }
}
