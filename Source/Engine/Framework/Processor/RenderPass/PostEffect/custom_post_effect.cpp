//===================================================
// custom_post_effect.cpp
// 
// Author: Miu Kitamura
// Date  : 2026/06/01
//===================================================
#include "custom_post_effect.h"

#include "Engine/engine_service_locator.h"

void CustomPostEffect::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // 状態を初期化
    m_state.Reset();
}

void CustomPostEffect::Finalize()
{
    m_state.Reset();
}

void CustomPostEffect::Process(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV)
{
    if (!m_pContext || !inputSRV || !outputRTV) return;

    // TODO: Add RadialBlur / MonoMask rendering here.
    // For now, preserve the current frame while this class is wired into the post effect chain.
    CopyInputToOutput(inputSRV, outputRTV);
    UnbindShaderResources();
}

void CustomPostEffect::CopyInputToOutput(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV)
{
    Direct3D_ResetViewport();
    Direct3D_ClearSceneTarget(outputRTV, nullptr);
    Direct3D_SetSceneTarget(outputRTV, nullptr);

    EngineServiceLocator::BindShader(ShaderBase::FullScreen);
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    m_pContext->PSSetShaderResources(0, 1, &inputSRV);
    m_pContext->Draw(3, 0);
}

void CustomPostEffect::UnbindShaderResources()
{
    ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 2, nullSRV);
}
