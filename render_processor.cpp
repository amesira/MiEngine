//===================================================
// render_processor.cpp [描画制御プロセッサー]
// 
// ・描画の制御を行うプロセッサー
// 
// Author：Miu Kitamura
// Date  ：2026/03/10
//===================================================
#include "render_processor.h"

#include "shader.h"
#include "sprite.h"
#include "direct3d.h"

#include "debug_renderer.h"

// 描画制御プロセッサーの初期化
void RenderProcessor::Initialize()
{
    m_lightingPass.Initialize();
    m_opaqueRenderPass.Initialize();
    m_uiRenderPass.Initialize();
}

// 描画制御プロセッサーの終了処理
void RenderProcessor::Finalize()
{
    m_lightingPass.Finalize();
    m_opaqueRenderPass.Finalize();
    m_uiRenderPass.Finalize();
}

// 描画制御プロセッサーの処理
void RenderProcessor::Process(IScene* pScene)
{
    if (!m_renderView) return; // RenderViewがバインドされていない場合は処理しない

    // インスタンシングデータをクリア
    ClearInstanceData();

    // シーンを初期化
    Direct3D_SetSceneRenderTarget(m_renderView->colorBufferRTV.Get());

    // 1.ライト設定パス
    m_lightingPass.Process(pScene);

    // 2.シャドウマップパス

    // 3.スカイボックスパス

    // 4.3Dオブジェクト描画
    m_lightingPass.SetLightEnable(m_renderView->enableLighting);
    SetDepthState(DEPTHSTATE_ENABLE);

    // 不透明物体
    m_opaqueRenderPass.BindMatrix(m_renderView->viewMatrix, m_renderView->projectionMatrix);
    m_opaqueRenderPass.Process(pScene);
    // 透明物体
    // m_transparentRenderPass.Process(pScene);

    m_lightingPass.SetLightEnable(false);

    // デバッグ描画
    DebugRenderer_DrawFlush(m_renderView->viewMatrix, m_renderView->projectionMatrix);

    SetDepthState(DEPTHSTATE_DISABLE);

    // 5.PostEffect描画
    if (m_renderView->enablePostEffect) {
        // ポストエフェクト描画処理
        // m_postEffectPass.Process(pScene);
    }

    // 6.2DScreen描画
    if (m_renderView->enableUI) {
        m_uiRenderPass.Process(pScene);
    }
}