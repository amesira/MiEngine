//---------------------------------------------------
// scene_settings.h
// ・リムライトや半球ライト、PostProcessの設定など、
// シーン全体に関する設定を保持するクラス。
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//---------------------------------------------------
#ifndef SCENE_SETTINGS_H
#define SCENE_SETTINGS_H
#include "lighting_settings.h"
#include "post_process_settings.h"
#include "scene_camera_settings.h"

class SceneSettings {
private: friend class SettingsViewWindow;
    SceneCameraSettings m_cameraSettings;          // シーン全体のカメラ設定
    LightingSettings m_lightingSettings;           // ライティング設定
    PostProcessSettings m_postProcessSettings;     // ポストプロセス設定

public:
    // カメラ設定の取得・設定
    const SceneCameraSettings& GetCameraSettings() const { return m_cameraSettings; }
    void SetCameraSettings(const SceneCameraSettings& settings) { m_cameraSettings = settings; }
    // カメラ設定の更新
    void UpdateCameraSettings() {
        m_cameraSettings.UpdateViewMatrix();
        m_cameraSettings.UpdateProjectionMatrix();
    }

    // ライティング設定の取得・設定
    const LightingSettings& GetLightingSettings() const { return m_lightingSettings; }
    void SetLightingSettings(const LightingSettings& settings) { m_lightingSettings = settings; }

    // ポストプロセス設定の取得・設定
    const PostProcessSettings& GetPostProcessSettings() const { return m_postProcessSettings; }
    void SetPostProcessSettings(const PostProcessSettings& settings) { m_postProcessSettings = settings; }

};

#endif // SCENE_SETTINGS_H