//---------------------------------------------------
// skybox_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/03
//---------------------------------------------------
#ifndef SKYBOX_PASS_H
#define SKYBOX_PASS_H
#include "pass.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "model_resource.h"

class SkyboxPass : public Pass {
private:
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;

    // SkyBox用のリソース
    ModelResource* m_pSkyboxModel;

    // 視点位置
    XMFLOAT3 m_eyePosition;

public:
    // 初期化
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    // 終了処理
    void Finalize() override;
    // 描画処理
    void Process(IScene* pScene) override;

    // 視点位置のセット
    void SetEyePosition(const XMFLOAT3& eyePos) { m_eyePosition = eyePos; }

};

#endif // SKYBOX_PASS_H