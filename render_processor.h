//----------------------------------------------------
// render_processor.h [描画制御プロセッサー]
// 
// ・描画の制御を行うプロセッサー
// 
// Author：Miu Kitamura
// Date  ：2026/03/10
//----------------------------------------------------
#ifndef RENDER_PROCESSOR_H
#define RENDER_PROCESSOR_H
#include "processor.h"

#include "render_view.h"

#include "lighting_pass.h"
#include "opaque_render_pass.h"
#include "ui_render_pass.h"

#include "direct3d.h"
#include "Windows.h"
#include "wrl/client.h"
using Microsoft::WRL::ComPtr;

class RenderProcessor : public Processor {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    ID3D11Buffer* m_pVertexBuffer = nullptr;

    // RenderViewへのポインタ
    RenderView* m_renderView;

    LightingPass m_lightingPass;
    OpaqueRenderPass m_opaqueRenderPass;
    // TransparentPass
    UIRenderPass m_uiRenderPass;

public:
    void    Initialize()override;
    void    Finalize()override;
    void    Process(IScene* pScene)override;

    // 描画に必要な情報をまとめる構造体RenderViewのバインド
    void    BindRenderView(RenderView* view) {
        m_renderView = view;
    }

    // SRVをフルスクリーンクワッドに描画する関数
    void    DrawFullScreenQuad(ID3D11ShaderResourceView* srv);

};

#endif // RENDER_PROCESSOR_H