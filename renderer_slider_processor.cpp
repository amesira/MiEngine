//===================================================
// renderer_slider_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/27
//===================================================
#include "renderer_slider_processor.h"
#include "scene_interface.h"
#include "game_object.h"

#include "direct3d.h"
#include "sprite.h"
#include "shader.h"

#include "rect_transform_component.h"
#include "slider_component.h"
#include "transform_component.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11ShaderResourceView* g_pTexture = nullptr;

void RendererSliderProcessor::Initialize()
{
    g_pDevice = Direct3D_GetDevice();
    g_pContext = Direct3D_GetDeviceContext();

    LoadTexture(&g_pTexture, L"asset\\Texture\\white.bmp");
}

void RendererSliderProcessor::Finalize()
{

}

void RendererSliderProcessor::Process(IScene* pScene)
{
    const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
    const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

    Shader_Begin();

    // 2D変換行列を設定
    XMMATRIX    vpMatrix2D = XMMatrixOrthographicOffCenterLH(
        0.0f,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0.0f,
        0.0f,
        1.0f);
    XMMATRIX    projection3D = Direct3D_GetProjectionMatrix();
    XMMATRIX    view3D = Direct3D_GetViewMatrix();

    auto* sliderComponentPool = pScene->GetComponentPool<SliderComponent>();
    auto* rectTransformPool = pScene->GetComponentPool<RectTransformComponent>();
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    if (sliderComponentPool == nullptr)return;

    auto& sliderList = sliderComponentPool->GetList();

    for (SliderComponent& slider : sliderList)
    {
        SliderComponent* pSlider = &slider;
        int gameObjectID = slider.GetOwner()->GetID();

        RectTransformComponent* pRect = rectTransformPool->GetByGameObjectID(gameObjectID);
        TransformComponent* pTransform = transformPool->GetByGameObjectID(gameObjectID);

        // コンポーネントが無効ならスキップ
        if (!pRect && !pTransform) continue;
        if (!pSlider->GetEnable()) continue;

        //----------------------------------------------------
        // BG描画
	    //----------------------------------------------------
        
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
        g_pContext->PSSetShaderResources(0, 1, &g_pTexture);

        // スプライト描画
        DrawSprite(pSlider->GetBgColor(),{0.0f, 0.0f, 1.0f, 1.0f});

        //----------------------------------------------------
        // 塗りつぶし描画
        //----------------------------------------------------
        XMMATRIX    adjustTranslation;
        float value = pSlider->GetValue();
        if (pRect) {
            scaleMatrix = XMMatrixScaling(
                pRect->GetScaling().x * value, pRect->GetScaling().y, 1.0f);
            adjustTranslation = XMMatrixTranslation(
                -pRect->GetScaling().x * 0.5f * (1.0f - value),
                0.0f,
                0.0f);
        }
        else if (pTransform) {
            scaleMatrix = XMMatrixScaling(
                pTransform->GetScaling().x * value, pTransform->GetScaling().y, 1.0f);
            adjustTranslation = XMMatrixTranslation(
                -pTransform->GetScaling().x * 0.5f * (1.0f - value),
                0.0f,
                0.0f);
        }
        worldMatrix = scaleMatrix * adjustTranslation * rotMatrix * transMatrix;

        // シェーダーに行列セット
        Shader_SetMatrix(worldMatrix * vpMatrix);
        Shader_SetWorldMatrix(worldMatrix);

        // テクスチャ設定
        g_pContext->PSSetShaderResources(0, 1, &g_pTexture);

        // スプライト描画
        DrawSprite(pSlider->GetFillColor(), { 0.0f, 0.0f, 1.0f, 1.0f });
    }
}