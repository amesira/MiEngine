//---------------------------------------------------
// decal_render_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/13
//---------------------------------------------------
#ifndef DECAL_RENDER_PASS_H
#define DECAL_RENDER_PASS_H
#include "Engine/Core/pass.h"
#include "Engine/System/Device/direct3d.h"
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

    // デプステクスチャのSRV
    ID3D11ShaderResourceView* m_depthSRV = nullptr;

public:
    ~DecalRenderPass() override = default;
    void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void    Finalize() override;
    void    Process(IScene* pScene) override;
    // デカール範囲のデバッグ描画
    void    CollectDebugDraw(IScene* pScene);

    // 深度情報のバインド
    void    SetDepthTexture(ID3D11ShaderResourceView* depthSRV);
    void    UnbindDepthTexture();

};

#endif // DECAL_RENDER_PASS_H