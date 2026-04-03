//===================================================
// render_processor.cpp [描画制御プロセッサー]
// 
// ・描画の制御を行うプロセッサー
// 
// Author：Miu Kitamura
// Date  ：2026/03/10
//===================================================
#include "render_processor.h"

#include "direct3d.h"
#include "shader_manager.h"
#include "engine_service_locator.h"

#include "debug_renderer.h"

// 描画制御プロセッサーの初期化
void RenderProcessor::Initialize()
{
    m_pDevice = Direct3D_GetDevice();
    m_pContext = Direct3D_GetDeviceContext();

    m_lightingPass.Initialize(m_pDevice, m_pContext);
    m_shadowMapPass.Initialize(m_pDevice, m_pContext);
    m_skyboxPass.Initialize(m_pDevice, m_pContext);
    m_opaqueRenderPass.Initialize(m_pDevice, m_pContext);
    m_uiRenderPass.Initialize(m_pDevice, m_pContext);
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

    //----------------------------------- 描画前の準備

    // 1.ライト設定パス
    m_lightingPass.Process(pScene);

    // 2.シャドウマップパス
    m_lightingPass.BindLightCB(false);

    m_shadowMapPass.SetEyePosition(m_renderView->eyePosition);
    m_shadowMapPass.SetLightDirection(m_lightingPass.GetDirectionalLightDirection()); // ライトの方向は適当

    m_shadowMapPass.Process(pScene);

    //----------------------------------- Scene描画開始
    Direct3D_SetSceneTarget(m_renderView->colorBufferRTV.Get(), m_renderView->depthBufferDSV.Get());

    // 3.スカイボックスパス
    //m_skyboxPass.SetEyePosition(m_renderView->eyePosition);
    //m_skyboxPass.Process(pScene);

    //----------------------------------- 3Dオブジェクト描画
    Bind3DCameraCB(m_renderView);

    m_lightingPass.BindLightCB(m_renderView->enableLighting);
    m_shadowMapPass.BindShadowLightCB();
    m_shadowMapPass.BindShadowMapSRV();

    // 不透明物体
    m_opaqueRenderPass.Process(pScene);

    // 透明物体
    // m_transparentRenderPass.Process(pScene);

    //m_lightingPass.SetLightEnable(false);

    //-----------------

    // デバッグ描画
    m_lightingPass.BindLightCB(false);
    DebugRenderer_DrawFlush(m_renderView->viewMatrix, m_renderView->projectionMatrix);

    //----------------------------------- 2Dオブジェクト描画
    Bind2DCameraCB(m_renderView);
    m_lightingPass.BindLightCB(false);

    // 5.PostEffect描画
    if (m_renderView->enablePostEffect) {
        // m_postEffectPass.Process(pScene);
    }

    // 6.2DScreen描画
    if (m_renderView->enableUI) {
        m_uiRenderPass.Process(pScene);
    }
}

// 3D描画時のカメラCBバインド
void RenderProcessor::Bind3DCameraCB(const RenderView* view)
{
    EngineServiceLocator::UpdateCameraCB({
        view->viewMatrix,
        view->projectionMatrix,
        XMFLOAT4(view->eyePosition.x, view->eyePosition.y, view->eyePosition.z, 1.0f)
        });
}

// 2D描画時のカメラCBバインド
void RenderProcessor::Bind2DCameraCB(const RenderView* view)
{
    const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
    const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

    EngineServiceLocator::UpdateCameraCB({
        XMMatrixIdentity(),
        XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f),
        XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f) // カメラの位置は適当（UIはワールド座標と同じなので）
        });
}