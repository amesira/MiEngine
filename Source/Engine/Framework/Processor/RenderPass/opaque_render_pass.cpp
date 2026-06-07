//===================================================
// opaque_render_pass.cpp
//
// Author: Miu Kitamura
// Date  : 2026/03/11
//===================================================
#include "opaque_render_pass.h"
#include "Engine/Core/scene_interface.h"

#include "Engine/Device/direct3d.h"
using namespace DirectX;

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"
#include "Engine/Framework/Processor/RenderPass/RenderUtility/model_render_utility.h"
#include "Engine/Framework/Processor/RenderPass/RenderUtility/sprite_render_utility.h"
#include "Engine/Graphics/shader_definitions.h"

#include "Engine/engine_service_locator.h"

#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()
#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()
#define SHADER_MANAGER EngineServiceLocator::GetShaderManager()

// OpaqueRenderPassの初期化処理
void OpaqueRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_defaultTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\white.bmp");

    // SpriteRenderer用の頂点バッファを生成
    {
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(ShaderDefinitions::SpriteVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer(&bd, NULL, &m_pSpriteVertexBuffer);
    }
}

// OpaqueRenderPassの終了処理
void OpaqueRenderPass::Finalize()
{

}

// OpaqueRenderPassの処理内容
void OpaqueRenderPass::Process(IScene* pScene, const RenderView& view)
{
    (void)view;
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_ENABLE);

    // モデルの描画
    ModelRenderUtility::ForEachRenderableModel(
        pScene,
        [this](ModelComponent& m, TransformComponent& t, ModelResource& model)
        {
            // ワールド行列の計算
            XMMATRIX worldMatrix = ModelRenderUtility::CreateWorldMatrix(t);
            EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

            // スキニングメッシュの場合はスキニングCBもバインド
            if (model.vertexType == ModelResource::VertexType::Skinned) {
                EngineServiceLocator::GetModelRepository()->BindSkinningCB(m.GetSkeletonPose().boneTransforms);
            }

            // ジオメトリの描画
            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            DrawMeshList(model.meshes, m.GetMaterialSlots());
        });

    // スプライトの描画
    SpriteRenderUtility::ForEachRenderableSprite(
        pScene,
        [this](SpriteRendererComponent& s, TransformComponent& t)
        {
            if (s.GetBlendMode() != SpriteRendererComponent::SpriteBlendMode::Opaque) return;
            const MaterialInstance& mat = s.GetMaterial();
            MaterialResource* material = mat.materialResource ? mat.materialResource : MATERIAL_REPOSITORY->GetMaterial("default");
            if (!material) return;
            if (material->renderMode != RenderMode::Opaque) return;

            if (material->shaderProgram != nullptr) {
                EngineServiceLocator::BindShader(material->shaderProgram);
            }
            else {
                EngineServiceLocator::BindShader(ShaderBase::SpriteLit);
            }

            // ワールド行列の計算
            XMMATRIX worldMatrix = SpriteRenderUtility::CreateWorldMatrix(t);
            EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

            MaterialInstance spriteMaterial = mat;
            spriteMaterial.materialResource = material;

            MaterialBufferData materialBufferData = material->CreateBufferData();
            materialBufferData.baseColor = spriteMaterial.isOverrideBaseColor ? spriteMaterial.overrideBaseColor : materialBufferData.baseColor;
            materialBufferData.emissiveColor = spriteMaterial.isOverrideEmissive ? spriteMaterial.overrideEmissiveColor : materialBufferData.emissiveColor;
            materialBufferData.emissiveIntensity = spriteMaterial.isOverrideEmissive ? spriteMaterial.overrideEmissiveIntensity : materialBufferData.emissiveIntensity;
            MATERIAL_REPOSITORY->BindMaterialCB(materialBufferData);
            MATERIAL_REPOSITORY->BindMaterialTexture(spriteMaterial);

            // UV矩形にフリップを適用
            XMFLOAT4 uvRect = SpriteRenderUtility::ApplyFlipToUvRect(s.GetUvRect(), s.GetFlipX(), s.GetFlipY());

            // 頂点バッファの更新
            if (!SpriteRenderUtility::UpdateSpriteVertexBuffer(m_pContext, m_pSpriteVertexBuffer, uvRect, s.GetColor())) {
                return;
            }

            UINT stride = sizeof(ShaderDefinitions::SpriteVertex);
            UINT offset = 0;
            m_pContext->IASetVertexBuffers(0, 1, &m_pSpriteVertexBuffer, &stride, &offset);
            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            m_pContext->Draw(4, 0);
        });
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

        // 不透明マテリアル以外はスキップ
        if (mat.materialResource->renderMode != RenderMode::Opaque)continue;

        if (mat.materialResource->shaderProgram != nullptr) {
            EngineServiceLocator::BindShader(mat.materialResource->shaderProgram);
        }

        // マテリアル定数バッファの更新とバインド
        MaterialBufferData materialBufferData = mat.materialResource->CreateBufferData();
        materialBufferData.baseColor = mat.isOverrideBaseColor ? mat.overrideBaseColor : materialBufferData.baseColor;
        materialBufferData.emissiveColor = mat.isOverrideEmissive ? mat.overrideEmissiveColor : materialBufferData.emissiveColor;
        materialBufferData.emissiveIntensity = mat.isOverrideEmissive ? mat.overrideEmissiveIntensity : materialBufferData.emissiveIntensity;
        MATERIAL_REPOSITORY->BindMaterialCB(materialBufferData);
        MATERIAL_REPOSITORY->BindMaterialTexture(mat);

        // カスタムプロパティのバインド
        XMFLOAT4 customProperties[MaterialResource::CUSTOM_PROPERTY_COUNT];
        mat.materialResource->GetCustomProperties(customProperties);
        MATERIAL_REPOSITORY->BindCustomProperties(customProperties);

        TextureResource* customTextures[MaterialResource::CUSTOM_TEXTURE_COUNT];
        mat.materialResource->GetCustomTextures(customTextures);
        MATERIAL_REPOSITORY->BindCustomTextures(customTextures);

        // ジオメトリの描画
        ModelRenderUtility::DrawMeshGeometry(m_pContext, mesh);
    }
}
