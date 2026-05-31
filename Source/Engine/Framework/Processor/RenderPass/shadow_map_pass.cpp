//===================================================
// shadow_map_pass.cpp
//
// Author: Miu Kitamura
// Date  : 2026/04/03
//===================================================
#include "shadow_map_pass.h"
#include "Engine/Core/scene_interface.h"

#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"
#include "Engine/Framework/Component/transform_component.h"

#include "Engine/Framework/Processor/RenderPass/RenderUtility/model_render_utility.h"
#include "Engine/Framework/Processor/RenderPass/RenderUtility/sprite_render_utility.h"

#include "Engine/Graphics/shader_definitions.h"

#include "Utility/mi_math.h"
#include "Engine/engine_service_locator.h"

#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()
#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define SHADER_MANAGER EngineServiceLocator::GetShaderManager()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

void ShadowMapPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_defaultTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\white.bmp");

    // シャドウマップ用のライト定数バッファを生成し、シェーダープログラムにバインド
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

    // シャドウマップ用の深度バッファと対応するビューを生成
    Direct3D_CreateDepthBuffer(depthBufferTexture.GetAddressOf(), depthBufferDSV.GetAddressOf(), depthBufferSRV.GetAddressOf());

    // スプライトをシャドウマップへ書き込むための頂点バッファを生成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(ShaderDefinitions::SpriteVertex) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(&bd, NULL, m_spriteVertexBuffer.GetAddressOf());
}

void ShadowMapPass::Finalize()
{

}

void ShadowMapPass::Process(IScene* pScene)
{
    // シャドウマップのレンダリング設定
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_ENABLE);
    SetRasterizerState(RASTERIZERSTATE_CULL_BACK);

    // シャドウマップ用のライトビュー行列と射影行列を計算して、ライト定数バッファに転送
    {
        float width = 50.0f;
        float height = 50.0f;

        XMFLOAT3 dir = MiMath::Normalize(m_lightDirection);
        XMFLOAT3 eye = MiMath::Subtract(m_eyePosition, MiMath::Multiply(dir, 30.0f));

        XMMATRIX viewMatrix = XMMatrixLookToLH(
            XMLoadFloat3(&eye),
            XMLoadFloat3(&dir),
            XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        XMMATRIX projectionMatrix = XMMatrixOrthographicLH(width, height, 0.1f, 200.0f);

        EngineServiceLocator::UpdateCameraCB({ viewMatrix, projectionMatrix, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) });
        m_shadowLightMatrix = viewMatrix * projectionMatrix;
    }

    //-------------------------------------
    // モデルの描画
    //-------------------------------------
    ModelResource::VertexType currentVertexType = ModelResource::VertexType::Static;
    EngineServiceLocator::BindShader(ShaderBase::Lit);
    m_pContext->PSSetShader(nullptr, nullptr, 0);

    ModelRenderUtility::ForEachRenderableModel(
        pScene,
        [this, &currentVertexType](ModelComponent& m, TransformComponent& t, ModelResource& model)
        {
            // シェーダーを切り替え
            if (currentVertexType != model.vertexType) {
                switch (model.vertexType) {
                case ModelResource::VertexType::Static:
                    EngineServiceLocator::BindShader(ShaderBase::Lit);
                    break;
                case ModelResource::VertexType::Skinned:
                    EngineServiceLocator::BindShader(ShaderBase::SkinnedLit);
                    break;
                }
                m_pContext->PSSetShader(nullptr, nullptr, 0);
                currentVertexType = model.vertexType;
            }

            // ワールド行列を計算
            XMMATRIX worldMatrix = ModelRenderUtility::CreateWorldMatrix(t);
            EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

            // スキニング行列を転送
            if (model.vertexType == ModelResource::VertexType::Skinned) {
                EngineServiceLocator::GetModelRepository()->BindSkinningCB(m.GetSkeletonPose().boneTransforms);
            }

            // ジオメトリの描画
            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            ModelRenderUtility::DrawMeshListGeometry(m_pContext, model.meshes);
        });

    //-------------------------------------
    // スプライトの描画
    //-------------------------------------
    EngineServiceLocator::BindShader(ShaderBase::SpriteUnlit);

    SpriteRenderUtility::ForEachRenderableSprite(
        pScene,
        [this](SpriteRendererComponent& s, TransformComponent& t)
        {
            if (s.GetBlendMode() == SpriteRendererComponent::SpriteBlendMode::AlphaBlend) return;
            if (s.GetBlendMode() == SpriteRendererComponent::SpriteBlendMode::Additive) return;

            // ライトから見て面が裏向きかどうか
            bool isBackFace = MiMath::Dot(t.GetForward(), m_lightDirection) < 0.0f;

            // ワールド行列を計算
            XMMATRIX worldMatrix = SpriteRenderUtility::CreateWorldMatrix(t);
            if (isBackFace) {
                // 裏面の場合は180度回転させる
                XMMATRIX flipRotation = XMMatrixRotationY(XMConvertToRadians(180.0f));
                worldMatrix = flipRotation * worldMatrix;
            }
            EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

            // テクスチャをバインド
            TextureResource* texture = s.GetTextureResource() ? s.GetTextureResource() : m_defaultTexture;
            if (texture) {
                m_pContext->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());
            }

            // UV矩形にフリップを適用して頂点バッファを更新
            XMFLOAT4 uvRect = SpriteRenderUtility::ApplyFlipToUvRect(s.GetUvRect(), s.GetFlipX(), s.GetFlipY());
            if (isBackFace) {
                // 裏面の場合はUVも反転させる
                uvRect = SpriteRenderUtility::ApplyFlipToUvRect(uvRect, true, false);
            }
            if (!SpriteRenderUtility::UpdateSpriteVertexBuffer(m_pContext, m_spriteVertexBuffer.Get(), uvRect, s.GetColor())) {
                return;
            }

            ID3D11Buffer* vertexBuffer = m_spriteVertexBuffer.Get();
            UINT stride = sizeof(ShaderDefinitions::SpriteVertex);
            UINT offset = 0;
            m_pContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

            // 描画
            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            m_pContext->Draw(4, 0);
        });
}

// ----------------------------------- Bind

// シャドウマップ用のライト定数バッファをシェーダーにバインド
void ShadowMapPass::BindShadowCB()
{
    XMMATRIX transposedMatrix = XMMatrixTranspose(m_shadowLightMatrix);
    m_pContext->UpdateSubresource(m_shadowLightCB->buffer.Get(), 0, nullptr, &transposedMatrix, 0, 0);
}

// シャドウマップ用の深度テクスチャをピクセルシェーダーにバインド
void ShadowMapPass::BindShadowTexture()
{
    m_pContext->PSSetShaderResources(10, 1, depthBufferSRV.GetAddressOf());
}

// シャドウマップ用の深度テクスチャをピクセルシェーダーから外す
void ShadowMapPass::UnbindShadowTexture()
{
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_pContext->PSSetShaderResources(10, 1, nullSRV);
}
