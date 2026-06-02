//---------------------------------------------------
// skybox_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/03
//---------------------------------------------------
#ifndef SKYBOX_PASS_H
#define SKYBOX_PASS_H
#include "Engine/Core/pass.h"

#include "Engine/Device/direct3d.h"
using namespace DirectX;
#include "Engine/Graphics/model_resource.h"

class SkyboxPass : public Pass {
private:
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;

    // SkyBox用のリソース
    ModelResource* m_pSkyboxModel;

public:
    // 初期化
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    // 終了処理
    void Finalize() override;
    // 描画処理
    void Process(IScene* pScene, const RenderView& view) override;

};

#endif // SKYBOX_PASS_H
