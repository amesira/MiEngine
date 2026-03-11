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
#include <DirectXMath.h>
using namespace DirectX;

class OpaqueRenderPass : public Pass {
private:
    XMMATRIX m_viewMatrix;
    XMMATRIX m_projectionMatrix;

public:
    void    Initialize() override;
    void    Finalize() override;
    void    Process(IScene* pScene) override;

    void    BindMatrix(XMMATRIX view, XMMATRIX projection) {
        m_viewMatrix = view;
        m_projectionMatrix = projection;
    }
};

#endif // OPAQUE_RENDER_PASS_H