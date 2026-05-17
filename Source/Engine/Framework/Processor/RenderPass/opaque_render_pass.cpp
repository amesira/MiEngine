//===================================================
// opaque_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//===================================================
#include "opaque_render_pass.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/Device/direct3d.h"
using namespace DirectX;

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/model_component.h"

#include "Engine/engine_service_locator.h"

#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

static TextureResource* s_testNormalTexture = nullptr;

// ホログラムシェーダーテスト
static ShaderProgramResource* s_hologramShader = nullptr;
static TextureResource* s_hologramNoiseTexture = nullptr;
static XMFLOAT4* s_hologramBuffer = new XMFLOAT4[8];

// OpaqueRenderPassの初期化
void OpaqueRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_defaultTexture = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Texture\\white.bmp");

    s_testNormalTexture = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Texture\\normal.png");

    // ホログラムシェーダーテスト
    ShaderProgramResource hologramShader;
    hologramShader.name = "HologramUnlit";
    hologramShader.baseShader = SHADER_REPOSITORY->GetShaderProgramResource(ShaderBase::Unlit);
    hologramShader.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource("hologram_unlit_ps.cso");
    s_hologramShader = SHADER_REPOSITORY->GenerateShaderProgramResource(hologramShader);
    s_hologramNoiseTexture = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Texture\\hologram_noise.png");
    
    s_hologramBuffer[0] = XMFLOAT4(0, 1, 1, 1);
    s_hologramBuffer[1] = XMFLOAT4(1, 0, 0, 0);
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

    // 描画ステートのセット
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_ENABLE);

    auto& modelPoolList = modelPool->GetList();

    // 通常モデル描画
    EngineServiceLocator::BindShader(s_hologramShader);
    m_pContext->PSSetShaderResources(5, 1, s_hologramNoiseTexture->texture.GetAddressOf());
    s_hologramBuffer[1].y += 0.016f;
    MATERIAL_REPOSITORY->BindCustomProperties(s_hologramBuffer);

    for (ModelComponent& m : modelPoolList) {
        ModelResource* model = m.GetModelResource();
        if (!model)continue;
        if (model->vertexType != ModelResource::VertexType::Static) continue;

        TransformComponent* t = transformPool->GetByGameObjectID(m.GetOwner()->GetID());

        // component無効チェック
        if (!t)continue;
        if (!m.GetEnable() || !t->GetEnable())continue;

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

        // Transformバッファをバインド
        EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

        // プリミティブトポロジ設定
        m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // メッシュリストの描画
        DrawMeshList(model->meshes, m.GetMaterialSlots());
    }

    // SkinnedModel描画
    EngineServiceLocator::BindShader(ShaderBase::SkinnedLit);
    
    for (ModelComponent& m : modelPoolList) {
        ModelResource* model = m.GetModelResource();
        if (!model)continue;
        if (model->vertexType != ModelResource::VertexType::Skinned) continue;

        TransformComponent* t = transformPool->GetByGameObjectID(m.GetOwner()->GetID());

        // component無効チェック
        if (!t)continue;
        if (!m.GetEnable() || !t->GetEnable())continue;

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

        // Transformバッファをバインド
        EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

        // スキニングCBバインド
        EngineServiceLocator::GetModelRepository()->BindSkinningCB(m.GetSkeletonPose().boneTransforms);

        // プリミティブトポロジ設定
        m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // メッシュリストの描画
        DrawMeshList(model->meshes, m.GetMaterialSlots());
    }

}

// ------------------------------------- private

// Meshリストの描画
void OpaqueRenderPass::DrawMeshList(const std::vector<ModelMesh>& meshes, const std::vector<MaterialInstance>& materialSlots)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        const ModelMesh& mesh = meshes[i];
        const MaterialInstance& mat = materialSlots[mesh.materialIndex];
        if (!mat.materialResource)continue;

        // Opeque以外は描画しない
        if (mat.materialResource->renderMode != RenderMode::Opaque)continue;

        // マテリアルバインド
        MaterialBufferData materialBufferData = mat.materialResource->CreateBufferData();
        materialBufferData.baseColor = mat.isOverrideBaseColor ? mat.overrideBaseColor : materialBufferData.baseColor;
        materialBufferData.emissiveColor = mat.isOverrideEmissiveColor ? mat.overrideEmissiveColor : materialBufferData.emissiveColor;
        MATERIAL_REPOSITORY->BindMaterialCB(materialBufferData);
        MATERIAL_REPOSITORY->BindMaterialTexture(*mat.materialResource);

        // 頂点バッファ設定
        m_pContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &mesh.vertexStride, &mesh.vertexOffset);

        // インデックスバッファ設定
        m_pContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        // ポリゴン描画
        m_pContext->DrawIndexed(mesh.numIndices, 0, 0);
    }
}
