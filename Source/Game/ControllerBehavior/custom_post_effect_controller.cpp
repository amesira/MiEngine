//===================================================
// custom_post_effect_controller.cpp
// 
// Author: Miu Kitamura
// Date  : 2026/06/02
//===================================================
#include "custom_post_effect_controller.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "Engine/Settings/scene_settings.h"

#include <algorithm>

#include "Engine/Device/mi_fps.h"

#include "game_controller_locator.h"
#include "Utility/debug_ostream.h"

CustomPostEffectController::CustomPostEffectController()
{
    s_instanceCount++;

    if (s_instanceCount > 1) {
        hal::dout << "Warning: CustomPostEffectController has multiple instances. Only one instance is expected." << std::endl;
        this->SetEnable(false);
    }
    else {
        GameControllerLocator::s_customPostEffectController = this;
    }

    m_state.Reset();
    m_state.radialBlur.sampleCount = 4; // デフォルトのサンプル数を設定
    m_state.monoMask.monoColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // デフォルトのモノクロ色を設定
}

CustomPostEffectController::~CustomPostEffectController()
{
    if (GameControllerLocator::s_customPostEffectController == this) {
        GameControllerLocator::s_customPostEffectController = nullptr;
    }

    s_instanceCount--;
}

void CustomPostEffectController::Start()
{

}

void CustomPostEffectController::Update()
{
    float deltaTime = FPS_GetUnscaledDeltaTime();

    // 各エフェクトのTweeningタスクを更新し、状態に反映させる
    for (int i = 0; i < static_cast<int>(CustomPostEffectType::MAX); i++) {
        FloatTweenTask& tweenTask = m_changeIntensityTask[i];
        bool isRunning = !tweenTask.IsFinished();

        tweenTask.Update(deltaTime);

        if (isRunning) {
            switch (static_cast<CustomPostEffectType>(i)) {
            case CustomPostEffectType::RadialBlur:
                m_state.radialBlur.strength = tweenTask.m_currentValue;
                break;
            case CustomPostEffectType::MonoMask:
                m_state.monoMask.strength = tweenTask.m_currentValue;
                break;
            default: break;
            }
        }
    }

    // カスタムポストエフェクト状態の更新
    IScene* scene = GetOwner()->GetScene();
    if (scene) {
        SceneSettings& sceneSettings = scene->GetSceneSettings();
        sceneSettings.GetPostProcessSettings().m_customPostEffectState = m_state;
    }
}

void CustomPostEffectController::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Custom Post Effect Controller")) {
        if (ImGui::TreeNode("Radial Blur")) {
            ImGui::DragInt("Sample Count", &m_state.radialBlur.sampleCount, 1.0f, 1, 64);
            ImGui::DragFloat("Strength", &m_state.radialBlur.strength, 0.01f, 0.0f, 1.0f);

            static float radialIntensity = 0.25f;
            static float radialDuration = 0.2f;
            static float radialHold = 0.1f;

            ImGui::DragFloat("Play Intensity", &radialIntensity, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Play Duration", &radialDuration, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Hold Duration", &radialHold, 0.01f, 0.0f, 5.0f);

            if (ImGui::Button("Play Radial Blur")) {
                PlayEffect(CustomPostEffectType::RadialBlur, radialIntensity, radialDuration, radialHold);
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset Radial Blur")) {
                m_state.radialBlur.strength = 0.0f;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Mono Mask")) {
            ImGui::ColorEdit4("Mono Color", &m_state.monoMask.monoColor.x);
            ImGui::DragFloat("Strength", &m_state.monoMask.strength, 0.01f, 0.0f, 1.0f);
            ImGui::Text("Mask Texture: %s", m_state.monoMaskTextureSRV ? "Set" : "None");

            static float monoIntensity = 1.0f;
            static float monoDuration = 0.2f;
            static float monoHold = 0.1f;

            ImGui::DragFloat("Play Intensity", &monoIntensity, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Play Duration", &monoDuration, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Hold Duration", &monoHold, 0.01f, 0.0f, 5.0f);

            if (ImGui::Button("Play Mono Mask")) {
                PlayEffect(CustomPostEffectType::MonoMask, monoIntensity, monoDuration, monoHold);
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset Mono Mask")) {
                m_state.monoMask.strength = 0.0f;
            }

            ImGui::TreePop();
        }

        if (ImGui::Button("Reset All Custom Effects")) {
            m_state.Reset();
            m_state.radialBlur.sampleCount = 4;
        }
    }

    InspectorViewWindow::EndComponentSection();
}

// ポストエフェクトの再生
void CustomPostEffectController::PlayEffect(
    CustomPostEffectType effectType, 
    float intensity, float duration, float holdDuration)
{
    int effectIndex = static_cast<int>(effectType);

    FloatTweenTask& tweenTask = m_changeIntensityTask[effectIndex];
    tweenTask.Reset();
    tweenTask.m_duration = duration;
    tweenTask.m_holdDuration = holdDuration;
    tweenTask.m_targetValue = intensity;
    tweenTask.m_endValue = 0.0f; // 終了後は強度0に戻す

    // 開始値は現在のエフェクトの強度にする
    switch (effectType) {
        case CustomPostEffectType::RadialBlur:
            tweenTask.m_startValue = m_state.radialBlur.strength;
            break;
        case CustomPostEffectType::MonoMask:
            tweenTask.m_startValue = m_state.monoMask.strength;
            break;
        default: break;
    }

    tweenTask.Start();
}
