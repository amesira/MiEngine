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
    ClearInstanceData();

    // ビュー行列とプロジェクション行列をシェーダーに設定
    Direct3D_SetViewMatrix(m_renderView.viewMatrix);
    Direct3D_SetProjectionMatrix(m_renderView.projectionMatrix);

    // ライト設定パス
    m_lightingPass.Process(pScene);

    SetDepthState(DEPTHSTATE_ENABLE);    // デプス有効

    // 3D描画パス
    m_lightingPass.SetLightEnable(m_renderView.enableLighting);
    m_opaqueRenderPass.Process(pScene);

    m_lightingPass.SetLightEnable(false);   // ライト無効

    SetDepthState(DEPTHSTATE_DISABLE); // デプス無効

    // UI描画
    if (m_renderView.enableUI) {
        m_uiRenderPass.Process(pScene);
    }
}
