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
using namespace DirectX;



class CustomPostEffectPass : public Pass {
public:
    // === カスタムポストエフェクトの状態を保持する構造体 ===
    struct RadialBlur {
        int sampleCount = 0;
        float strength = 0.0f;
    };
    struct MonoMask {
        XMFLOAT4 monoColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        float strength = 0.0f;
    };

    struct CustomPostEffectState 
    {
        RadialBlur radialBlur;

        MonoMask monoMask;
        ID3D11ShaderResourceView* m_monoMaskTextureSRV = nullptr;

        void Reset() {
            radialBlur.sampleCount = 0;
            radialBlur.strength = 0.0f;
            monoMask.monoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            monoMask.strength = 0.0f;
        }
    };

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // 入力と出力のビュー
    ID3D11ShaderResourceView* m_inputSRV = nullptr;
    ID3D11RenderTargetView* m_outputRTV = nullptr;

    // カスタムポストエフェクトの状態
    CustomPostEffectState m_state;

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
