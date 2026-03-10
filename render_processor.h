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

class RenderProcessor : public Processor {
private:
    // SceneRenderPass
    // 
    // SkyboxPass
    // OpaquePass
    // TransparentPass
    // 
    // PostEffectPass
    // 
    // 
    // UIRenderPass

public:
    void    Initialize()override;
    void    Finalize()override;
    void    Process(IScene* pScene)override;

};

#endif // RENDER_PROCESSOR_H