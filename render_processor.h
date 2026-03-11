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

class RenderProcessor : public Processor {
private:
    RenderView m_renderView;

    // LightingPass
    LightingPass m_lightingPass;
    // ShadowMapPass
    // 
    // SkyboxPass
    // OpaqueRenderPass
    OpaqueRenderPass m_opaqueRenderPass;
    // TransparentPass
    // 
    // ←Snapshot !
    // 
    // PostEffectPass
    // 
    // ←Snapshot !
    // 
    // UIRenderPass
    UIRenderPass m_uiRenderPass;
    // 
    // ←Snapshot !

public:
    void    Initialize()override;
    void    Finalize()override;
    void    Process(IScene* pScene)override;

    // 描画に必要な情報をまとめる構造体RenderViewのバインド
    void    BindRenderView(const RenderView& view) {
        m_renderView = view;
    }

};

#endif // RENDER_PROCESSOR_H