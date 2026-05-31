//===================================================
// shadow_map_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/03
//===================================================
#include "shadow_map_pass.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/model_component.h"

#include "Utility/mi_math.h"
#include "Engine/engine_service_locator.h"
#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define SHADER_MANAGER EngineServiceLocator::GetShaderManager()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

void ShadowMapPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // シャドウマップ用のライト定数バッファ作成
    m_shadowLightCB = SHADER_REPOSITORY->GenerateConstantBufferResource(
        "ShadowLightBuffer",
        11,
        sizeof(XMMATRIX),
        true,
        true,
        ConstantBufferUsage::Default);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::Lit)], m_shadowLightCB);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::SkinnedLit)], m_shadowLightCB);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::SpriteLit)], m_shadowLightCB);

    // シャドウマップ用のリソース作成
    Direct3D_CreateDepthBuffer(depthBufferTexture.GetAddressOf(), depthBufferDSV.GetAddressOf(), depthBufferSRV.GetAddressOf());
}

void ShadowMapPass::Finalize()
{

}

void ShadowMapPass::Process(IScene* pScene)
{
    // コンポーネントプール取得
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* modelPool = pScene->GetComponentPool<ModelComponent>();
    if (!transformPool || !modelPool)return;

    // 描画ステートのセット
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_ENABLE);

    // シェーダーの初期セット
    ModelResource::VertexType currentVertexType = ModelResource::VertexType::Static;
    EngineServiceLocator::BindShader(ShaderBase::Lit);
    m_pContext->PSSetShader(nullptr, nullptr, 0);

    // カメラCBの更新
    float width = 50.0f;
    float height = 50.0f;

    XMFLOAT3 center = m_eyePosition;
    XMFLOAT3 dir = MiMath::Normalize(m_lightDirection);
    XMFLOAT3 eye = MiMath::Subtract(m_eyePosition, MiMath::Multiply(dir, 30.0f));

    XMMATRIX viewMatrix = XMMatrixLookToLH(
        XMLoadFloat3(&eye),
        XMLoadFloat3(&dir),
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    XMMATRIX projectionMatrix = XMMatrixOrthographicLH(width, height, 0.1f, 200.0f);

    EngineServiceLocator::UpdateCameraCB({ viewMatrix, projectionMatrix, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)});
    m_shadowLightMatrix = viewMatrix * projectionMatrix;

    // モデル描画
    auto& modelPoolList = modelPool->GetList();
    for (ModelComponent& m : modelPoolList) {
        TransformComponent* t = transformPool->GetByGameObjectID(m.GetOwner()->GetID());

        // component無効チェック
        if (!t)continue;
        if (!m.GetEnable() || !t->GetEnable())continue;

        // モデルデータ取得
        ModelResource* model = m.GetModelResource();
        if (!model)continue;

        // シェーダー切替
        if (currentVertexType != model->vertexType) {
            switch (model->vertexType) {
            case ModelResource::VertexType::Static:
                EngineServiceLocator::BindShader(ShaderBase::Lit);
                break;
            case ModelResource::VertexType::Skinned:
                EngineServiceLocator::BindShader(ShaderBase::SkinnedLit);
                break;
            }
            currentVertexType = model->vertexType;
        }

        // ワールド行列計算
        XMMATRIX worldMatrix = XMMatrixIdentity();
        {
            XMMATRIX scaling = XMMatrixScaling(
                t->GetScaling().x,
                t->GetScaling().y,
                t->GetScaling().z);
            XMMATRIX rotation = XMMatrixRotationQuaternion(t->GetRotationVector());
            XMMATRIX translation = XMMatrixTranslation(
                t->GetPosition().x,
                t->GetPosition().y,
                t->GetPosition().z);

            worldMatrix = scaling * rotation * translation;
        }

        // 行列セット
        EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

        // スキニングCBバインド
        if (currentVertexType == ModelResource::VertexType::Skinned) {
            EngineServiceLocator::GetModelRepository()->BindSkinningCB(m.GetSkeletonPose().boneTransforms);
        }

        UINT stride = 0;
        UINT offset = 0;
        switch (model->vertexType) {
        case ModelResource::VertexType::Static:
            stride = sizeof(ModelVertex);
            break;
        case ModelResource::VertexType::Skinned:
            stride = sizeof(SkinnedModelVertex);
            break;
        }

        // プリミティブトポロジ設定
        m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        for (unsigned int i = 0; i < model->meshes.size(); i++)
        {
            ModelMesh& mesh = model->meshes[i];

            // 頂点バッファ設定
            m_pContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);

            // インデックスバッファ設定
            m_pContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

            // ポリゴン描画
            m_pContext->DrawIndexed(mesh.numIndices, 0, 0);
        }
    }
}

// ----------------------------------- Bind

// シャドウライトCBのバインド
void ShadowMapPass::BindShadowCB()
{
    XMMATRIX transposedMatrix = XMMatrixTranspose(m_shadowLightMatrix);

    // シャドウマップ用のライト行列を定数バッファに更新
    m_pContext->UpdateSubresource(m_shadowLightCB->buffer.Get(), 0, nullptr, &transposedMatrix, 0, 0);
}

// シャドウマップのSRVのバインド
void ShadowMapPass::BindShadowTexture()
{
    m_pContext->PSSetShaderResources(10, 1, depthBufferSRV.GetAddressOf());
}

// シャドウマップのSRVを外す
void ShadowMapPass::UnbindShadowTexture()
{
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_pContext->PSSetShaderResources(10, 1, nullSRV);
}
