//===================================================
// custom_post_effect_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/01
//===================================================
#include "custom_post_effect_pass.h"

#include "Engine/Core/scene_interface.h"
#include "Engine/engine_service_locator.h"

void CustomPostEffectPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    m_state.Reset();
}

void CustomPostEffectPass::Finalize()
{
    m_inputSRV = nullptr;
    m_outputRTV = nullptr;
    m_state.Reset();
}

void CustomPostEffectPass::Process(IScene* pScene)
{
    if (!m_pContext || !m_inputSRV || !m_outputRTV) return;

    // TODO: RadialBlur / Mask / Monochrome の実描画をここへ追加する。
    // 現段階では、パスを挿し込んでも画面が変わらないようにコピーだけ行う。
    CopyInputToOutput();
    UnbindShaderResources();
}

void CustomPostEffectPass::CopyInputToOutput()
{
    Direct3D_ResetViewport();
    Direct3D_ClearSceneTarget(m_outputRTV, nullptr);
    Direct3D_SetSceneTarget(m_outputRTV, nullptr);

    EngineServiceLocator::BindShader(ShaderBase::FullScreen);
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    m_pContext->PSSetShaderResources(0, 1, &m_inputSRV);
    m_pContext->Draw(3, 0);
}

void CustomPostEffectPass::UnbindShaderResources()
{
    ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 2, nullSRV);
}
