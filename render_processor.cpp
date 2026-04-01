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

    // インスタンシングデータをクリア
    //ClearInstanceData();

    // シーンを初期化
    Direct3D_SetSceneRenderTarget(m_renderView->colorBufferRTV.Get());

    // 1.ライト設定パス
  //  m_lightingPass.Process(pScene);

    // 2.シャドウマップパス

    // 3.スカイボックスパス

    // 4.3Dオブジェクト描画
    m_lightingPass.SetLightEnable(m_renderView->enableLighting);
    SetDepthState(DEPTHSTATE_ENABLE);

    //// 不透明物体
    m_opaqueRenderPass.BindMatrix(m_renderView->viewMatrix, m_renderView->projectionMatrix);
    m_opaqueRenderPass.Process(pScene);
    //// 透明物体
    //// m_transparentRenderPass.Process(pScene);

    //m_lightingPass.SetLightEnable(false);

    //// デバッグ描画
    ////ClearInstanceData();
    //DebugRenderer_DrawFlush(m_renderView->viewMatrix, m_renderView->projectionMatrix);

    //SetDepthState(DEPTHSTATE_DISABLE);

    // 5.PostEffect描画
    if (m_renderView->enablePostEffect) {
        // ポストエフェクト描画処理
        // m_postEffectPass.Process(pScene);
    }

    // 6.2DScreen描画
    if (m_renderView->enableUI) {
        m_lightingPass.SetLightEnable(false);
        m_uiRenderPass.Process(pScene);
    }
}

// SRVをフルスクリーンクワッドに描画する関数
void RenderProcessor::DrawFullScreenQuad(ID3D11ShaderResourceView** srv)
{
    SetBlendState(BLENDSTATE_ALFA);
    SetDepthState(DEPTHSTATE_DISABLE);

    EngineServiceLocator::BindShader(ShaderManager::ShaderType::Default);

    float SCREEN_WIDTH = static_cast<float>(Direct3D_GetBackBufferWidth());
    float SCREEN_HEIGHT = static_cast<float>(Direct3D_GetBackBufferHeight());

    // フルスクリーンクワッドの頂点データ
    D3D11_MAPPED_SUBRESOURCE msr;
    m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    Vertex* v = (Vertex*)msr.pData;

    v[0].position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
    v[0].texCoord = XMFLOAT2(0.0f, 0.0f);
    
    v[1].position = XMFLOAT3(0.5f, -0.5f , 0.0f);
    v[1].texCoord = XMFLOAT2(1.0f, 0.0f);

    v[2].position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
    v[2].texCoord = XMFLOAT2(0.0f, 1.0f);

    v[3].position = XMFLOAT3(0.5f, 0.5f, 0.0f);
    v[3].texCoord = XMFLOAT2(1.0f, 1.0f);

    for (int i = 0; i < 4; i++) {
        v[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        v[i].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
    }

    m_pContext->Unmap(m_pVertexBuffer, 0);

    // 頂点バッファの設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // プリミティブトポロジ設定 トライアングルストリップ
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // シェーダーリソースビューをセット
    m_pContext->PSSetShaderResources(0, 1, srv);

    XMMATRIX world = XMMatrixIdentity();
    {
        XMMATRIX translate = XMMatrixTranslation(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.0f);
        XMMATRIX scale = XMMatrixScaling(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
        world = scale * translate;
    }

    // ポリゴン描画命令発行
    EngineServiceLocator::UpdateTransformCB({ world, XMMatrixIdentity() });
    EngineServiceLocator::UpdateCameraCB({
         XMMatrixIdentity(), // ビュー行列は単位行列（UIはワールド座標と同じ）
         XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f), // プロジェクション行列は正射影
         XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f) // カメラの位置は適当（UIはワールド座標と同じなので）
        });
    m_pContext->Draw(4, 0);
}
