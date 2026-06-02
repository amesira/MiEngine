//---------------------------------------------------
// transparent_render_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//---------------------------------------------------
#ifndef TRANSPARENT_RENDER_PASS_H
#define TRANSPARENT_RENDER_PASS_H
#include "Engine/Core/pass.h"

#include "Engine/Device/direct3d.h"
#include "Engine/Graphics/texture_resource.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class ParticleSystemComponent;

class TransparentRenderPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // デフォルトテクスチャ
    TextureResource* m_defaultTexture = nullptr;

    // ParticleRenderer用の頂点バッファ
    ComPtr<ID3D11Buffer> m_pParticleVertexBuffer;
    ComPtr<ID3D11Buffer> m_pParticleInstanceBuffer;

public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Finalize() override;
    void Process(IScene* pScene, const RenderView& view) override;

private:
    // パーティクルシステムの描画
    void DrawParticleSystem(ParticleSystemComponent& particleSystem, const RenderView& view);

};

#endif // TRANSPARENT_RENDER_PASS_H
