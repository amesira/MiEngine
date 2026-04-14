//---------------------------------------------------
// decal_render_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/13
//---------------------------------------------------
#ifndef DECAL_RENDER_PASS_H
#define DECAL_RENDER_PASS_H
#include "pass.h"

#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

class ModelResource;

// デカールレンダリングパス
class DecalRenderPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // デカール描画用のモデルリソース
    ModelResource* m_decalCubeResource = nullptr;

public:
    ~DecalRenderPass() override = default;
    void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void    Finalize() override;
    void    Process(IScene* pScene) override;

};

#endif // DECAL_RENDER_PASS_H