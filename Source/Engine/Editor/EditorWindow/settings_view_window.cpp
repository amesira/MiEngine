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
    SceneCameraSettings cameraSettings = m_editorContext->sceneSettings->GetCameraSettings();

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

    m_editorContext->sceneSettings->SetCameraSettings(cameraSettings);
}

// ライティングの設定UIを描画
void SettingsViewWindow::DrawLightingSettings()
{
    ImGui::Text("Lighting Settings");
    LightingSettings lightingSettings = m_editorContext->sceneSettings->GetLightingSettings();

    ImGui::Separator();

    ImGui::Text("Rim Light Settings");
    ImGui::PushID("RimLightSettings");
    {
        RimLightSettings rimLightSettings = lightingSettings.GetRimLightSettings();
        
        // リムライトの有効/無効
        bool rimEnabled = rimLightSettings.enabled;
        if (ImGui::Checkbox("Enabled", &rimEnabled)) {
            rimLightSettings.enabled = rimEnabled;
        }

        // リムライトの強さ
        float rimIntensity = rimLightSettings.intensity;
        if (ImGui::DragFloat("Intensity", &rimIntensity, 0.1f, 0.0f, 10.0f)) {
            rimLightSettings.intensity = rimIntensity;
        }

        // リムライトの閾値
        float rimThreshold = rimLightSettings.threshold;
        if (ImGui::DragFloat("Threshold", &rimThreshold, 0.01f, 0.0f, 1.0f)) {
            rimLightSettings.threshold = rimThreshold;
        }

        // リムライトの色
        XMFLOAT3 rimColor = rimLightSettings.color;
        if (ImGui::ColorEdit3("Color", &rimColor.x)) {
            rimLightSettings.color = rimColor;
        }

        lightingSettings.SetRimLightSettings(rimLightSettings);
    }
    ImGui::PopID();

    ImGui::Separator();

    ImGui::Text("Hemisphere Light Settings");
    ImGui::PushID("HemisphereLightSettings");
    {
        HemisphereLightSettings hemisphereLightSettings = lightingSettings.GetHemisphereLightSettings();
        // 半球ライトの有効/無効
        bool hemiEnabled = hemisphereLightSettings.enabled;
        if (ImGui::Checkbox("Enabled", &hemiEnabled)) {
            hemisphereLightSettings.enabled = hemiEnabled;
        }
        // 半球ライトの強さ
        float hemiIntensity = hemisphereLightSettings.intensity;
        if (ImGui::DragFloat("Intensity", &hemiIntensity, 0.1f, 0.0f, 10.0f)) {
            hemisphereLightSettings.intensity = hemiIntensity;
        }
        // 半球ライトの空の色
        XMFLOAT3 hemiSkyColor = hemisphereLightSettings.skyColor;
        if (ImGui::ColorEdit3("Sky Color", &(hemiSkyColor.x))) {
            hemisphereLightSettings.skyColor = hemiSkyColor;
        }
        // 半球ライトの地面の色
        XMFLOAT3 hemiGroundColor = hemisphereLightSettings.groundColor;
        if (ImGui::ColorEdit3("Ground Color", &(hemiGroundColor.x))) {
            hemisphereLightSettings.groundColor = hemiGroundColor;
        }
        lightingSettings.SetHemisphereLightSettings(hemisphereLightSettings);
    }
    ImGui::PopID();

    m_editorContext->sceneSettings->SetLightingSettings(lightingSettings);
}

// PostProcessingの設定UIを描画
void SettingsViewWindow::DrawPostProcessingSettings()
{
    ImGui::Text("Post Processing Settings");

}