//---------------------------------------------------
// post_process_settings.h
// 
// 1. Tone Mapping
// 2. Bloom
// 3. Color Grading / LUT
// 4. Vignette
// 5. FXAA　→　アンチエイリアス
// 6. SSAO　→　アンビエントオクルージョン
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//---------------------------------------------------
#ifndef POST_PROCESS_SETTINGS_H
#define POST_PROCESS_SETTINGS_H
#include "Engine/Device/direct3d.h"
using namespace DirectX;

#pragma region SubSettings
// ToneMapping設定構造体
struct ToneMappingSettings {
    bool    enabled = false;               // トーンマッピングの有効/無効
    float   exposure = 1.0f;              // トーンマッピングの露出
};

// Bloom設定構造体
struct BloomSettings {
    bool    enabled = false;               // ブルームの有効/無効
    float   intensity = 1.0f;              // ブルームの強さ
    float   threshold = 1.0f;              // ブルームの閾値（明るさのしきい値）
};

// Color Grading / LUT設定構造体
struct ColorGradingSettings {
    bool    enabled = false;               // カラーグレーディングの有効/無効
    // LUTテクスチャなどの設定を追加
};

// Vignette設定構造体
struct VignetteSettings {
    bool    enabled = false;               // ビネットの有効/無効
    float   intensity = 1.0f;              // ビネットの強さ
    float   radius = 0.5f;                // ビネットの半径
};

// FXAA設定構造体
struct FXAASettings {
    bool    enabled = false;               // FXAAの有効/無効
    // その他のFXAAパラメータを追加
};

// SSAO設定構造体
struct SSAOSettings {
    bool    enabled = false;               // SSAOの有効/無効
    float   radius = 0.5f;                // SSAOの半径
    float   bias = 0.025f;               // SSAOのバイアス
    int     sampleCount = 16;             // SSAOのサンプル数
};

#pragma endregion


// カスタムポストエフェクトの状態を保持する構造体
struct CustomPostEffectState {
    // RadialBlur
    struct RadialBlur {
        int sampleCount = 4;
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

// PostProcessの設定クラス
class PostProcessSettings {
public:
    ToneMappingSettings m_toneMappingSettings;   // トーンマッピング設定
    BloomSettings m_bloomSettings;               // ブルーム設定
    ColorGradingSettings m_colorGradingSettings; // カラーグレーディング設定
    VignetteSettings m_vignetteSettings;         // ビネット設定
    FXAASettings m_fxaaSettings;                 // FXAA設定
    SSAOSettings m_ssaoSettings;               // SSAO設定

    CustomPostEffectState m_customPostEffectState; // カスタムポストエフェクトの状態

};

#endif // POST_PROCESS_SETTINGS_H