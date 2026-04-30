//---------------------------------------------------
// settings_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/30
//---------------------------------------------------
#ifndef SETTINGS_VIEW_WINDOW_H
#define SETTINGS_VIEW_WINDOW_H
#include "imgui_window_interface.h"

class SettingsViewWindow : public IImguiWindow {
private:

public:
    SettingsViewWindow(EditorContext* editorContext) : IImguiWindow(editorContext) {}
    void Draw() override;

private:
    // シーンカメラの設定UIを描画
    void DrawSceneCameraSettings();
    // ライティングの設定UIを描画
    void DrawLightingSettings();
    // PostProcessingの設定UIを描画
    void DrawPostProcessingSettings();

};

#endif // SETTINGS_VIEW_WINDOW_H