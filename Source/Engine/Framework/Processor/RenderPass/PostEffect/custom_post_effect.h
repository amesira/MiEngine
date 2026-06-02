//---------------------------------------------------
// custom_post_effect.h
// 
// Author: Miu Kitamura
// Date  : 2026/06/01
//---------------------------------------------------
#ifndef CUSTOM_POST_EFFECT_H
#define CUSTOM_POST_EFFECT_H
// direct3d
#include "Engine/Device/direct3d.h"
using namespace DirectX;
// comptr
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class ShaderProgramResource;
class ConstantBufferResource;

class IScene;

// カスタムポストエフェクト用の定数バッファ構造体
struct CustomPostEffectBuffer {
    union {
        XMFLOAT4 data[8];
        struct RadialBlur {
            int sampleCount;   // サンプル数
            float strength;    // ブラーの強さ
            float padding[2];  // パディング
        } radialBlur;
        struct MonoMask {
            XMFLOAT4 monoColor; // モノクロ化する色
            float strength;     // モノクロ化の強さ
            float padding[3];   // パディング
        } monoMask;
    };
    void Reset() {
        for (int i = 0; i < 8; i++) {
            data[i] = XMFLOAT4(0, 0, 0, 0);
        }
    }
};

class CustomPostEffect {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // ポストプロセス用の定数バッファデータ
    CustomPostEffectBuffer m_postProcessBufferData;

    // === シェーダーリソース ===
    ShaderProgramResource* m_radialBlurShader;  // ラジアルブラー用のシェーダー
    ShaderProgramResource* m_monoMaskShader;    // モノクロマスク用のシェーダー

    ConstantBufferResource* m_postProcessCB;    // ポストプロセス用の定数バッファ

    // 一時バッファ用のテクスチャ
    ComPtr<ID3D11Texture2D> m_tempTexture[2];
    ComPtr<ID3D11RenderTargetView> m_tempRTV[2];
    ComPtr<ID3D11ShaderResourceView> m_tempSRV[2];

public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Finalize();
    void Process(IScene* scene, ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV);

private:
    // 定数バッファの更新
    void UpdateConstantBuffer();

    // RadialBlurの処理
    void RadialBlur(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV, 
        CustomPostEffectBuffer::RadialBlur radialBlur);
    // MonoMaskの処理
    void MonoMask(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV,
        CustomPostEffectBuffer::MonoMask monoMask, ID3D11ShaderResourceView* monoMaskSRV);

};

#endif // CUSTOM_POST_EFFECT_H
