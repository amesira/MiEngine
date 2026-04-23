//===================================================
// skybox_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/03
//===================================================
#include "skybox_pass.h"
#include "Engine/engine_service_locator.h"

void SkyboxPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    return;

    // SkyBox用のモデルリソースをロード
    m_pSkyboxModel = EngineServiceLocator::GetModelRepository()->GetModel("asset\\Model\\skybox.fbx");
}

void SkyboxPass::Finalize()
{

}

void SkyboxPass::Process(IScene* pScene)
{
    return;
    // 描画ステートのセット
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_ENABLE);

    // ワールド行列計算
    XMMATRIX worldMatrix = XMMatrixIdentity();
    {
        XMMATRIX scaling = XMMatrixScaling(100.0f, 100.0f, 100.0f);
        XMMATRIX translation = XMMatrixTranslation(m_eyePosition.x, m_eyePosition.y, m_eyePosition.z);
        worldMatrix = scaling * translation;
    }

    // TransformCBの更新
    EngineServiceLocator::UpdateTransformCB({ worldMatrix, XMMatrixIdentity() });

    // プリミティブトポロジ設定
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}