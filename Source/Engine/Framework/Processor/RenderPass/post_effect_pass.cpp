//===================================================
// post_effect_pass.cpp
//
// Author: Miu Kitamura
// Date  : 2026/04/29
//===================================================
#include "post_effect_pass.h"

#include "Engine/Core/scene_interface.h"

// ポストエフェクト初期化
void PostEffectPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_postProcess.Initialize(m_pDevice, m_pContext);
}

// ポストエフェクト終了
void PostEffectPass::Finalize()
{
    m_postProcess.Finalize();
}

// ポストエフェクト処理
void PostEffectPass::Process(IScene* pScene)
{
    // PostProcess
    m_postProcess.Process(m_inputSRV, m_outputRTV);
}
