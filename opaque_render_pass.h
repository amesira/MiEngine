//---------------------------------------------------
// opaque_render_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//---------------------------------------------------
#ifndef OPAQUE_RENDER_PASS_H
#define OPAQUE_RENDER_PASS_H
#include "pass.h"
#include <vector>

class OpaqueRenderPass : public Pass {
public:
    void    Initialize() override;
    void    Finalize() override;
    void    Process(IScene* pScene) override;

};

#endif // OPAQUE_RENDER_PASS_H