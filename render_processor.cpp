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

    m_lightingPass.Initialize(Direct3D_GetDevice(), Direct3D_GetDeviceContext());
    m_opaqueRenderPass.Initialize();
    m_uiRenderPass.Initialize(Direct3D_GetDevice(), Direct3D_GetDeviceContext());

    // 頂点バッファ生成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * 4; // フルスクリーンクワッドの頂点数
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(&bd, NULL, &m_pVertexBuffer);
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

    // シーンを初期化
    Direct3D_SetSceneRenderTarget(m_renderView->colorBufferRTV.Get());

    // 1.ライト設定パス
    m_lightingPass.Process(pScene);

    // 2.シャドウマップパス

    // 3.スカイボックスパス

    // 4.3Dオブジェクト描画
    Bind3DCameraCB(m_renderView);
    m_lightingPass.BindLightCB(m_renderView->enableLighting);

    // 不透明物体
    m_opaqueRenderPass.Process(pScene);

    //// 透明物体
    //// m_transparentRenderPass.Process(pScene);

    //m_lightingPass.SetLightEnable(false);

    // デバッグ描画
    DebugRenderer_DrawFlush(m_renderView->viewMatrix, m_renderView->projectionMatrix);

    // 5.PostEffect描画
    if (m_renderView->enablePostEffect) {
        // ポストエフェクト描画処理
        // m_postEffectPass.Process(pScene);
    }

    // 6.2DScreen描画
    Bind2DCameraCB(m_renderView);
    if (m_renderView->enableUI) {
        m_lightingPass.BindLightCB(false);
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