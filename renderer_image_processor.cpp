//===================================================
// renderer_image_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
#include "renderer_image_processor.h"

#include "direct3d.h"
#include "sprite.h"
#include "shader.h"
using namespace DirectX;

#include "scene_interface.h"
#include "game_object.h"
#include "rect_transform_component.h"
#include "image_component.h"
#include "transform_component.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

void RendererImageProcessor::Initialize()
{
    g_pDevice = Direct3D_GetDevice();
    g_pContext = Direct3D_GetDeviceContext();
}

void RendererImageProcessor::Finalize()
{

}

void RendererImageProcessor::Process(IScene* pScene)
{
    Shader_Begin();
    SetBlendState(BLENDSTATE_ALFA);

    const float screenWidth = (float)Direct3D_GetBackBufferWidth();
    const float screenHeight = (float)Direct3D_GetBackBufferHeight();

    XMMATRIX vpMatrix2D = XMMatrixOrthographicOffCenterLH(
        0.0f,
        screenWidth,
        screenHeight,
        0.0f,
        0.0f,
        1.0f);
    XMMATRIX view3D = Direct3D_GetViewMatrix();
    XMMATRIX projection3D = Direct3D_GetProjectionMatrix();

    auto* imagePool = pScene->GetComponentPool<ImageComponent>();
    auto* rectTransformPool = pScene->GetComponentPool<RectTransformComponent>();
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    if(imagePool == nullptr)return;

    auto& imageList = imagePool->GetList();

    for(ImageComponent& image : imageList) {
        ImageComponent* pImage = &image;
        if (!pImage->GetEnable()) continue;

        // コンポーネントが無効ならスキップ
        unsigned int gameObjectID = image.GetOwner()->GetID();
        RectTransformComponent* pRect = nullptr;
        if(rectTransformPool){
            pRect = rectTransformPool->GetByGameObjectID(gameObjectID);
        }
        TransformComponent* pTransform = nullptr;
        if(transformPool){
            pTransform = transformPool->GetByGameObjectID(gameObjectID);
        }
        if (!pRect && !pTransform) continue;
        if (pRect && (!pRect->GetEnable() || !m_drawUiImages)) continue;
        if (pTransform && (!pTransform->GetEnable() || !m_drawWorldImages)) continue;
        if (image.GetOwner()->GetActive() == false) continue;

        // ワールド座標計算
        XMMATRIX scaleMatrix, rotMatrix, transMatrix;
        if (pRect) {
            scaleMatrix = XMMatrixScaling(
                pRect->GetScaling().x, pRect->GetScaling().y, 1.0f);
            rotMatrix = XMMatrixRotationRollPitchYaw(
                0.0f, 0.0f, XMConvertToRadians(pRect->GetRotation().z));
            transMatrix = XMMatrixTranslation(
                pRect->GetPosition().x,
                pRect->GetPosition().y,
                0.0f);
        }
        else if (pTransform) {
            scaleMatrix = XMMatrixScaling(
                pTransform->GetScaling().x, pTransform->GetScaling().y, 1.0f);
            rotMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(180.0f));

            // ビルボード表示
            if (pImage->GetWorldSpaceType() == WorldSpaceType::Billboard) {
                rotMatrix *= XMMatrixScaling(-1.0f, 1.0f, 1.0f);
                transMatrix = view3D;
                {
                    transMatrix.r[3].m128_f32[0] = 0.0f;
                    transMatrix.r[3].m128_f32[1] = 0.0f;
                    transMatrix.r[3].m128_f32[2] = 0.0f;
                    transMatrix.r[3].m128_f32[3] = 1.0f;
                }
                transMatrix = XMMatrixTranspose(transMatrix); // 転置行列にする
                {
                    transMatrix.r[3].m128_f32[0] = pTransform->GetPosition().x;
                    transMatrix.r[3].m128_f32[1] = pTransform->GetPosition().y;
                    transMatrix.r[3].m128_f32[2] = pTransform->GetPosition().z;
                    transMatrix.r[3].m128_f32[3] = 1.0f;
                }
            }
            // HD2D表示
            else if (pImage->GetWorldSpaceType() == WorldSpaceType::HD2D) {
                XMFLOAT3 camForward = Direct3D_GetCameraForward();
                float angleY = atan2f(camForward.x, camForward.z);
                rotMatrix *= XMMatrixRotationY(angleY + XM_PI);
                transMatrix = XMMatrixTranslation(
                    pTransform->GetPosition().x,
                    pTransform->GetPosition().y,
                    pTransform->GetPosition().z);
            }
            // 通常のワールド配置
            else { // transformの回転を用いる
                rotMatrix *= XMMatrixRotationQuaternion(pTransform->GetRotation());
                transMatrix = XMMatrixTranslation(
                    pTransform->GetPosition().x,
                    pTransform->GetPosition().y,
                    pTransform->GetPosition().z);
            }
            
        }
        XMMATRIX worldMatrix = scaleMatrix * rotMatrix * transMatrix;

        // vp計算
        XMMATRIX vpMatrix;
        if (pRect) {
            vpMatrix = vpMatrix2D;
        }
        else {
            vpMatrix = view3D * projection3D;
        }

        // シェーダーに行列セット
        Shader_SetMatrix(worldMatrix * vpMatrix);
        Shader_SetWorldMatrix(worldMatrix);

        // テクスチャ設定
        ID3D11ShaderResourceView* texture = pImage->GetTexture();
        g_pContext->PSSetShaderResources(0, 1, &texture);

        // スプライト描画
        DrawSprite(pImage->GetColor(), pImage->GetUvRect());
    }
}
