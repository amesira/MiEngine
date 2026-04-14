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
#include "wrl/client.h"
using Microsoft::WRL::ComPtr;

class ModelResource;

// デカールレンダリングパス
class DecalRenderPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // デカール描画用のモデルリソース
    ModelResource* m_decalCubeResource = nullptr;

    // 深度情報のSRV
    ComPtr<ID3D11ShaderResourceView> m_depthSRV;

public:
    ~DecalRenderPass() override = default;
    void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void    Finalize() override;
    void    Process(IScene* pScene) override;

    // 深度情報のセット
    void    SetDepthSRV(ID3D11ShaderResourceView* depthSRV) {
        m_depthSRV = depthSRV;
    }

};

#endif // DECAL_RENDER_PASS_H