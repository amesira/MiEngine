//===================================================
// post_effect_pass.cpp
//
// Author: Miu Kitamura
// Date  : 2026/04/29
//===================================================
#include "post_effect_pass.h"

#include "Engine/Core/scene_interface.h"
#include "Engine/render_view.h"

// ポストエフェクト初期化
void PostEffectPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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

    m_postProcess.Initialize(m_pDevice, m_pContext);
    m_customPostEffect.Initialize(m_pDevice, m_pContext);
}

// ポストエフェクト終了
void PostEffectPass::Finalize()
{
    m_postProcess.Finalize();
    m_customPostEffect.Finalize();
}

// ポストエフェクト処理
void PostEffectPass::Process(IScene* pScene, const RenderView& view)
{
    // PostProcess
    m_postProcess.Process(view.colorBufferSRV.Get(), m_tempRTV[0].Get());

    // CustomPostEffect
    m_customPostEffect.Process(pScene, m_tempSRV[0].Get(), view.postEffectRTV.Get(), view.maskColorBufferSRV.Get());
}
