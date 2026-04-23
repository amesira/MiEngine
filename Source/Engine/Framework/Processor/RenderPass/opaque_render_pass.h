//---------------------------------------------------
// opaque_render_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//---------------------------------------------------
#ifndef OPAQUE_RENDER_PASS_H
#define OPAQUE_RENDER_PASS_H
#include "Engine/Core/pass.h"
#include <vector>

#include "Engine/System/Graphics/texture_resource.h"
#include "Engine/System/Graphics/model_resource.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

class OpaqueRenderPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    TextureResource* m_defaultTexture = nullptr;

public:
    void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void    Finalize() override;
    void    Process(IScene* pScene) override;

private:

};

#endif // OPAQUE_RENDER_PASS_H