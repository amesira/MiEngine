//---------------------------------------------------
// particle_render_utility.h
//---------------------------------------------------
#ifndef PARTICLE_RENDER_UTILITY_H
#define PARTICLE_RENDER_UTILITY_H

#include "Engine/Device/direct3d.h"
#include "Engine/Framework/Component/particle_system_component.h"

struct RenderView;

namespace ParticleRenderUtility {
    // パーティクルのクワッド頂点バッファを更新する関数
    bool UpdateParticleQuadVertexBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* vertexBuffer);

    // ビルボードの回転行列を作成する関数
    DirectX::XMMATRIX CreateBillboardRotation(
        ParticleSystemData::BillboardMode billboardMode,
        const RenderView& view);

    // パーティクルのインスタンスバッファを更新する関数
    int UpdateParticleInstanceBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* instanceBuffer,
        const ParticleSystemComponent& particleSystem,
        const DirectX::XMMATRIX& billboardRotation);

}

#endif // PARTICLE_RENDER_UTILITY_H
