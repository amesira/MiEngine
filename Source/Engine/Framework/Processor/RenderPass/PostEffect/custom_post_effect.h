//---------------------------------------------------
// custom_post_effect.h
// 
// Author: Miu Kitamura
// Date  : 2026/06/01
//---------------------------------------------------
#ifndef CUSTOM_POST_EFFECT_H
#define CUSTOM_POST_EFFECT_H

#include "Engine/Device/direct3d.h"
using namespace DirectX;

// カスタムポストエフェクトの状態を保持する構造体
struct CustomPostEffectState {
    // RadialBlur
    struct RadialBlur {
        int sampleCount = 0;
        float strength = 0.0f;
    };
    RadialBlur radialBlur;
    // MonoMask
    struct MonoMask {
        XMFLOAT4 monoColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        float strength = 0.0f;
    };
    MonoMask monoMask;
    ID3D11ShaderResourceView* monoMaskTextureSRV = nullptr;

    // 状態のリセット
    void Reset() {
        radialBlur.sampleCount = 0;
        radialBlur.strength = 0.0f;
        monoMask.monoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        monoMask.strength = 0.0f;
        monoMaskTextureSRV = nullptr;
    }
};

class CustomPostEffect {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // エフェクトの状態
    CustomPostEffectState m_state;

public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Finalize();
    void Process(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV);

    // エフェクトの状態の設定・取得
    void SetState(const CustomPostEffectState& state) { m_state = state; }
    CustomPostEffectState& GetState() { return m_state; }
    const CustomPostEffectState& GetState() const { return m_state; }

private:
    void CopyInputToOutput(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV);
    void UnbindShaderResources();
};

#endif // CUSTOM_POST_EFFECT_H
