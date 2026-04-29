//---------------------------------------------------
// lighting_settings.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//---------------------------------------------------
#ifndef LIGHTING_SETTINGS_H
#define LIGHTING_SETTINGS_H
#include <DirectXMath.h>
using namespace DirectX;

// リムライト設定構造体
struct RimLightSettings {
    bool    enabled = false;               // リムライトの有効/無効
    float   intensity = 1.0f;              // リムライトの強さ
    float   threshold = 0.5f;              // リムライトの閾値（法線と視線の角度）
    XMFLOAT3 color = { 1.0f, 1.0f, 1.0f }; // リムライトの色
};

// 半球ライト設定構造体
struct HemisphereLightSettings {
    bool    enabled = false;                    // 半球ライトの有効/無効
    float   intensity = 1.0f;                   // 半球ライトの強さ
    XMFLOAT3 skyColor = { 0.5f, 0.5f, 0.5f };   // 空の色
    XMFLOAT3 groundColor = { 0.2f, 0.2f, 0.2f };// 地面の色
};

// ライティング設定クラス
class LightingSettings {
private:
    RimLightSettings m_rimLightSettings;               // リムライト設定
    HemisphereLightSettings m_hemisphereLightSettings; // 半球ライト設定

public:
    // リムライト設定の取得・設定
    const RimLightSettings& GetRimLightSettings() const { return m_rimLightSettings; }
    void SetRimLightSettings(const RimLightSettings& settings) { m_rimLightSettings = settings; }

    // 半球ライト設定の取得・設定
    const HemisphereLightSettings& GetHemisphereLightSettings() const { return m_hemisphereLightSettings; }
    void SetHemisphereLightSettings(const HemisphereLightSettings& settings) { m_hemisphereLightSettings = settings; }

};

#endif // LIGHTING_SETTINGS_H