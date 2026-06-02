//===================================================
// custom_post_effect.cpp
// 
// Author: Miu Kitamura
// Date  : 2026/06/01
//===================================================
#include "custom_post_effect.h"

#include "Engine/engine_service_locator.h"
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

#include "Engine/Core/scene_interface.h"
#include "Engine/Settings/scene_settings.h"

void CustomPostEffect::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // 画面サイズに合わせた一時バッファを作成する
    unsigned int SCREEN_WIDTH = Direct3D_GetBackBufferWidth();
    unsigned int SCREEN_HEIGHT = Direct3D_GetBackBufferHeight();

    for (int i = 0; i < 2; i++) {
        Direct3D_CreateColorBuffer(
            m_tempTexture[i].GetAddressOf(),
            m_tempRTV[i].GetAddressOf(),
            m_tempSRV[i].GetAddressOf(),
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        );
    }

    ShaderProgramResource* fullScreenShader = SHADER_REPOSITORY->GetShaderProgramResource(ShaderBase::FullScreen);

    m_postProcessCB = SHADER_REPOSITORY->GetConstantBufferResource("PostProcessBuffer");

    // シェーダーの読み込み
    ShaderProgramResource radialBlurShaderResource;
    radialBlurShaderResource.name = "RadialBlur";
    radialBlurShaderResource.baseShader = fullScreenShader;
    radialBlurShaderResource.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource("radial_blur_ps.cso");
    m_radialBlurShader = SHADER_REPOSITORY->GenerateShaderProgramResource(radialBlurShaderResource);

    ShaderProgramResource monoMaskShader;
    monoMaskShader.name = "MonoMask";
    monoMaskShader.baseShader = fullScreenShader;
    monoMaskShader.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource("mono_mask_ps.cso");
    m_monoMaskShader = SHADER_REPOSITORY->GenerateShaderProgramResource(monoMaskShader);
}

void CustomPostEffect::Finalize()
{
    
}

void CustomPostEffect::Process(IScene* scene, ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV)
{
    if (!m_pContext || !inputSRV || !outputRTV) return;

    Direct3D_ClearSceneTarget(outputRTV, nullptr, 1.0f);

    const CustomPostEffectState& customEffectState = scene->GetSceneSettings().GetPostProcessSettings().m_customPostEffectState;
    
    // MonoMaskの処理
    CustomPostEffectBuffer::MonoMask monoMaskParams = {};
    monoMaskParams.monoColor = customEffectState.monoMask.monoColor;
    monoMaskParams.strength = customEffectState.monoMask.strength;
    MonoMask(inputSRV, m_tempRTV[0].Get(),monoMaskParams, customEffectState.monoMaskTextureSRV);

    // RadialBlurの処理
    CustomPostEffectBuffer::RadialBlur radialBlurParams = {};
    radialBlurParams.sampleCount = customEffectState.radialBlur.sampleCount;
    radialBlurParams.strength = customEffectState.radialBlur.strength;
    RadialBlur(m_tempSRV[0].Get(), outputRTV, radialBlurParams);
}

// -------------------------------- private
// 定数バッファの更新
void CustomPostEffect::UpdateConstantBuffer()
{
    // union内の現在有効なパラメータをそのまま定数バッファへ転送する
    D3D11_MAPPED_SUBRESOURCE msr = {};
    m_pContext->Map(m_postProcessCB->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    XMFLOAT4* cbData = reinterpret_cast<XMFLOAT4*>(msr.pData);
    for (int i = 0; i < 8; i++) {
        cbData[i] = m_postProcessBufferData.data[i];
    }
    m_pContext->Unmap(m_postProcessCB->buffer.Get(), 0);
}

// -------------------------------- private - エフェクトの種類ごとの処理
// RadialBlurの処理
void CustomPostEffect::RadialBlur(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV,
    CustomPostEffectBuffer::RadialBlur radialBlur)
{
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    EngineServiceLocator::BindShader(m_radialBlurShader);

    // 定数バッファの更新
    m_postProcessBufferData.Reset();
    m_postProcessBufferData.radialBlur.sampleCount = radialBlur.sampleCount;
    m_postProcessBufferData.radialBlur.strength = radialBlur.strength;
    UpdateConstantBuffer();

    // 入力テクスチャをセット
    m_pContext->PSSetShaderResources(0, 1, &inputSRV);

    // 描画
    Direct3D_ResetViewport();
    Direct3D_ClearSceneTarget(outputRTV, nullptr, 1.0f);
    Direct3D_SetSceneTarget(outputRTV, nullptr);
    m_pContext->Draw(3, 0);
}

// MonoMaskの処理
void CustomPostEffect::MonoMask(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV,
    CustomPostEffectBuffer::MonoMask monoMask, ID3D11ShaderResourceView* monoMaskSRV)
{
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    EngineServiceLocator::BindShader(m_monoMaskShader);

    // 定数バッファの更新
    m_postProcessBufferData.Reset();
    m_postProcessBufferData.monoMask.monoColor = monoMask.monoColor;
    m_postProcessBufferData.monoMask.strength = monoMask.strength;
    UpdateConstantBuffer();

    // 入力テクスチャをセット
    m_pContext->PSSetShaderResources(0, 1, &inputSRV);
    m_pContext->PSSetShaderResources(1, 1, &monoMaskSRV);

    // 描画
    Direct3D_ResetViewport();
    Direct3D_ClearSceneTarget(outputRTV, nullptr, 1.0f);
    Direct3D_SetSceneTarget(outputRTV, nullptr);
    m_pContext->Draw(3, 0);
}