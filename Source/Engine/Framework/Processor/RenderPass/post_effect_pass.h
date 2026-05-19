//---------------------------------------------------
// post_effect_pass.h
//
// Author: Miu Kitamura
// Date  : 2026/04/29
//---------------------------------------------------
#ifndef POST_EFFECT_PASS_H
#define POST_EFFECT_PASS_H
#include "Engine/Core/pass.h"
#include "Engine/Device/direct3d.h"

#include "./PostEffect/post_process.h"

class PostEffectPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // 入力のSRVと出力のRTV
    ID3D11ShaderResourceView* m_inputSRV = nullptr;
    ID3D11RenderTargetView* m_outputRTV = nullptr;

    // PostProcessのインスタンス
    PostProcess m_postProcess;

public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Finalize() override;
    void Process(IScene* pScene) override;

    void SetInputSRV(ID3D11ShaderResourceView* inputSRV) { m_inputSRV = inputSRV; }
    void SetOutputRTV(ID3D11RenderTargetView* outputRTV) { m_outputRTV = outputRTV; }

};

#endif // POST_EFFECT_PASS_H
