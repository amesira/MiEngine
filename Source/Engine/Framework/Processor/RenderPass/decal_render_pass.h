//---------------------------------------------------
// decal_render_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/13
//---------------------------------------------------
#ifndef DECAL_RENDER_PASS_H
#define DECAL_RENDER_PASS_H
#include "Engine/Core/pass.h"
#include "Engine/Device/direct3d.h"
using namespace DirectX;

#include "wrl/client.h"
using Microsoft::WRL::ComPtr;

class ModelResource;
class ShaderProgramResource;

// デカールレンダリングパス
class DecalRenderPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // デカール描画用のモデルリソース
    ModelResource* m_decalCubeResource = nullptr;

    // デカール描画用のシェーダープログラムリソース
    ShaderProgramResource* m_decalShader = nullptr;

public:
    ~DecalRenderPass() override = default;
    void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void    Finalize() override;
    void    Process(IScene* pScene, const RenderView& view) override;

    // デカール範囲のデバッグ描画
    void    CollectDebugDraw(IScene* pScene);

    // 深度情報のバインド
    void    UnbindDepthTexture();

};

#endif // DECAL_RENDER_PASS_H
