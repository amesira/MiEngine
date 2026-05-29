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
#include "Engine/Framework/Component/sprite_renderer_component.h"

#include "Engine/engine_service_locator.h"

#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()
#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()
#define SHADER_MANAGER EngineServiceLocator::GetShaderManager()

// OpaqueRenderPassの初期化
void OpaqueRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_defaultTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\white.bmp");

    // SpriteRenderer用の頂点バッファ生成
    {
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(SpriteVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer(&bd, NULL, &m_pSpriteVertexBuffer);
    }
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
    if (transformPool && modelPool){
        auto& modelPoolList = modelPool->GetList();

        // 描画ステートのセット
        SetBlendState(BLENDSTATE_NONE);
        SetDepthState(DEPTHSTATE_ENABLE);

        for (ModelComponent& m : modelPoolList) {
            ModelResource* model = m.GetModelResource();
            if (!model)continue;

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
            if (model->vertexType == ModelResource::VertexType::Skinned) {
                EngineServiceLocator::GetModelRepository()->BindSkinningCB(m.GetSkeletonPose().boneTransforms);
            }

            // プリミティブトポロジ設定
            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // メッシュリストの描画
            DrawMeshList(model->meshes, m.GetMaterialSlots());
        }
    }

    auto* spriteRendererPool = pScene->GetComponentPool<SpriteRendererComponent>();
    if (transformPool && spriteRendererPool) {
        auto& spriteRendererPoolList = spriteRendererPool->GetList();

        // 描画ステートのセット
        SetBlendState(BLENDSTATE_NONE);
        SetDepthState(DEPTHSTATE_ENABLE);

        // Shader設定
        EngineServiceLocator::BindShader(ShaderBase::SpriteLit);

        for (SpriteRendererComponent& s : spriteRendererPoolList) {
            TransformComponent* t = transformPool->GetByGameObjectID(s.GetOwner()->GetID());

            // component無効チェック
            if (!t)continue;
            if (!s.GetEnable() || !t->GetEnable())continue;

            // Opeque以外は描画しない
            if (s.GetBlendMode() != SpriteRendererComponent::SpriteBlendMode::Opaque)continue;


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
                XMMATRIX flipRotation = XMMatrixRotationY(XM_PI);
                worldMatrix = scaling * flipRotation * rotation * translation;
            }

            // Transformバッファをバインド
            EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

            // プリミティブトポロジ設定
            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

            // テクスチャのセット
            TextureResource* texture = s.GetTextureResource() ? s.GetTextureResource() : m_defaultTexture;
            m_pContext->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());

            XMFLOAT4 uvRect = s.GetUvRect();
            XMFLOAT4 color = s.GetColor();

            // 頂点バッファ設定
            D3D11_MAPPED_SUBRESOURCE msr;
            m_pContext->Map(m_pSpriteVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
            {
                float h = 0.5f; // スプライトの半分のサイズ
                SpriteVertex* v = (SpriteVertex*)msr.pData;
                v[0].position = XMFLOAT3(-h, -h, 0.0f);
                v[1].position = XMFLOAT3(h, -h, 0.0f);
                v[2].position = XMFLOAT3(-h, h, 0.0f);
                v[3].position = XMFLOAT3(h, h, 0.0f);
                v[0].texCoord = XMFLOAT2(uvRect.x, uvRect.y + uvRect.w);
                v[1].texCoord = XMFLOAT2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
                v[2].texCoord = XMFLOAT2(uvRect.x, uvRect.y);
                v[3].texCoord = XMFLOAT2(uvRect.x + uvRect.z, uvRect.y);
                for (int i = 0; i < 4; i++) {
                    v[i].color = color;
                    v[i].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
                }
            }
            UINT stride = sizeof(SpriteVertex);
            UINT offset = 0;
            m_pContext->IASetVertexBuffers(0, 1, &m_pSpriteVertexBuffer, &stride, &offset);

            m_pContext->Unmap(m_pSpriteVertexBuffer, 0);

            // ポリゴン描画
            m_pContext->Draw(4, 0);
        }
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

        if (mat.materialResource->shaderProgram != nullptr){
            EngineServiceLocator::BindShader(mat.materialResource->shaderProgram);
        }

        // マテリアルバインド
        MaterialBufferData materialBufferData = mat.materialResource->CreateBufferData();
        materialBufferData.baseColor = mat.isOverrideBaseColor ? mat.overrideBaseColor : materialBufferData.baseColor;
        materialBufferData.emissiveColor = mat.isOverrideEmissive ? mat.overrideEmissiveColor : materialBufferData.emissiveColor;
        materialBufferData.emissiveIntensity = mat.isOverrideEmissive ? mat.overrideEmissiveIntensity : materialBufferData.emissiveIntensity;
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
