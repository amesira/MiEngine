//===================================================
// settings_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/30
//===================================================
#include "settings_view_window.h"
#include "Engine/Editor/editor_context.h"

#include "Engine/Settings/scene_settings.h"


void SettingsViewWindow::Draw()
{
    if (ImGui::BeginTabBar("Settings View Group")) {
        ImVec2 avail = ImGui::GetContentRegionAvail();

        if (ImGui::BeginTabItem("Scene Camera")) {
            DrawSceneCameraSettings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Lighting")) {
            DrawLightingSettings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Post Processing")) {
            DrawPostProcessingSettings();
            ImGui::EndTabItem();
        }

    }
    ImGui::EndTabBar();
}

// ----------------------------- private

// シーンカメラの設定UIを描画
void SettingsViewWindow::DrawSceneCameraSettings()
{
    ImGui::Text("Scene Camera Settings");

    SceneCameraSettings& cameraSettings = m_editorContext->sceneSettings->m_cameraSettings;

    // カメラの位置を表示
    XMFLOAT3 pos = cameraSettings.GetPosition();
    if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
        cameraSettings.SetPosition(pos);
    }
    // 注視点を表示
    XMFLOAT3 atPos = cameraSettings.GetAtPosition();
    if (ImGui::DragFloat3("At Position", &atPos.x, 0.1f)) {
        cameraSettings.SetAtPosition(atPos);
    }

    ImGui::Separator();

    // 視野角を表示
    float fov = cameraSettings.GetFov();
    if (ImGui::DragFloat("FOV", &fov, 1.0f, 1.0f, 179.0f)) {
        cameraSettings.SetFov(fov);
    }
}

// ライティングの設定UIを描画
void SettingsViewWindow::DrawLightingSettings()
{
    ImGui::Text("Lighting Settings");

}

// PostProcessingの設定UIを描画
void SettingsViewWindow::DrawPostProcessingSettings()
{
    ImGui::Text("Post Processing Settings");

}