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

#include "model_resource.h"

#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

class OpaqueRenderPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

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

private:
    void    DrawModel(ModelResource* model, const XMMATRIX& world, const XMFLOAT4& color);

};

#endif // OPAQUE_RENDER_PASS_H