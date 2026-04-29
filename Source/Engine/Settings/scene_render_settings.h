//---------------------------------------------------
// scene_render_settings.h
// ・リムライトや半球ライト、PostProcessの設定など、
// シーン全体のレンダリングに関する設定を保持するクラス。
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//---------------------------------------------------
#ifndef SCENE_RENDER_SETTINGS_H
#define SCENE_RENDER_SETTINGS_H
#include "lighting_settings.h"
#include "post_process_settings.h"

class SceneRenderSettings {
private:
    LightingSettings m_lightingSettings;           // ライティング設定
    PostProcessSettings m_postProcessSettings;     // ポストプロセス設定

public:
    // ライティング設定の取得・設定
    const LightingSettings& GetLightingSettings() const { return m_lightingSettings; }
    void SetLightingSettings(const LightingSettings& settings) { m_lightingSettings = settings; }

    // ポストプロセス設定の取得・設定
    const PostProcessSettings& GetPostProcessSettings() const { return m_postProcessSettings; }
    void SetPostProcessSettings(const PostProcessSettings& settings) { m_postProcessSettings = settings; }

};

#endif // SCENE_RENDER_SETTINGS_H