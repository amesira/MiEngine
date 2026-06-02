//---------------------------------------------------
// mask_render_pass.h
//
// Author: Miu Kitamura
// Date  : 2026/06/03
//---------------------------------------------------
#ifndef MASK_RENDER_PASS_H
#define MASK_RENDER_PASS_H

#include "Engine/Core/pass.h"
#include "Engine/Device/direct3d.h"
#include "Engine/Graphics/model_resource.h"
#include "Engine/Graphics/texture_resource.h"

class MaskRenderPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // デフォルトテクスチャ
    TextureResource* m_defaultTexture = nullptr;

    // スプライト描画用の頂点バッファ
    ID3D11Buffer* m_pSpriteVertexBuffer = nullptr;

public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Finalize() override;
    void Process(IScene* pScene, const RenderView& view) override;

private:
    // メッシュの描画
    void DrawMeshList(const std::vector<ModelMesh>& meshes);
};

#endif // MASK_RENDER_PASS_H
