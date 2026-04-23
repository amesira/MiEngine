//---------------------------------------------------
// shadow_map_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/03
//---------------------------------------------------
#ifndef SHADOW_MAP_PASS_H
#define SHADOW_MAP_PASS_H
#include "Engine/Core/pass.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

#include "Windows.h"
#include "wrl/client.h"
using Microsoft::WRL::ComPtr;

#include "Engine/System/Graphics/texture_resource.h"

class ShadowMapPass : public Pass {
private:
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;

    // シャドウマップ用のライト定数バッファ
    ComPtr<ID3D11Buffer> m_shadowLightCB;
    XMMATRIX m_shadowLightMatrix;

    // シャドウマップ用のリソース
    ComPtr<ID3D11Texture2D>             depthBufferTexture;
    ComPtr<ID3D11DepthStencilView>      depthBufferDSV;
    ComPtr<ID3D11ShaderResourceView>    depthBufferSRV;

    // 視点位置（シャドウマップの中心位置として使用）
    XMFLOAT3 m_eyePosition;
    // ライト方向（シャドウマップの視野方向として使用）
    XMFLOAT3 m_lightDirection;

public:
    ~ShadowMapPass() override = default;
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Finalize() override;
    void Process(IScene* pScene) override;

    // DepthStencilViewの取得
    ID3D11DepthStencilView* GetDepthStencilView() const { return depthBufferDSV.Get(); }

    // 視点位置のセット
    void SetEyePosition(const XMFLOAT3& eyePos) { m_eyePosition = eyePos; }
    // ライト方向のセット
    void SetLightDirection(const XMFLOAT3& lightDir) { m_lightDirection = lightDir; }

    // シャドウマップ用のバインド
    void BindShadowCB();
    void BindShadowTexture();
    // シャドウマップ用テクスチャを外す
    void UnbindShadowTexture();

};

#endif // SHADOW_MAP_PASS_H