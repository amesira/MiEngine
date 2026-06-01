//---------------------------------------------------
// custom_post_effect_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/01
//---------------------------------------------------
#ifndef CUSTOM_POST_EFFECT_PASS_H
#define CUSTOM_POST_EFFECT_PASS_H

#include "Engine/Core/pass.h"
#include "Engine/Device/direct3d.h"

struct CustomPostEffectState {
    float radialBlurIntensity = 0.0f;
    float monochromeIntensity = 0.0f;
    float maskIntensity = 0.0f;
    ID3D11ShaderResourceView* maskSRV = nullptr;

    void Reset() {
        radialBlurIntensity = 0.0f;
        monochromeIntensity = 0.0f;
        maskIntensity = 0.0f;
        maskSRV = nullptr;
    }

    bool IsActive() const {
        return radialBlurIntensity > 0.0f
            || monochromeIntensity > 0.0f
            || (maskIntensity > 0.0f && maskSRV);
    }
};

class CustomPostEffectPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    ID3D11ShaderResourceView* m_inputSRV = nullptr;
    ID3D11RenderTargetView* m_outputRTV = nullptr;

    CustomPostEffectState m_state = {};

public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Finalize() override;
    void Process(IScene* pScene) override;

    void SetInputSRV(ID3D11ShaderResourceView* inputSRV) { m_inputSRV = inputSRV; }
    void SetOutputRTV(ID3D11RenderTargetView* outputRTV) { m_outputRTV = outputRTV; }
    void SetState(const CustomPostEffectState& state) { m_state = state; }
    CustomPostEffectState& GetState() { return m_state; }
    const CustomPostEffectState& GetState() const { return m_state; }

private:
    void CopyInputToOutput();
    void UnbindShaderResources();
};

#endif // CUSTOM_POST_EFFECT_PASS_H
