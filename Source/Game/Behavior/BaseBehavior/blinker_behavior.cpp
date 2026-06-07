//===================================================
// blinker_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/07
//===================================================
#include "blinker_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"
#include "Engine/Graphics/material_resource.h"
#include "Utility/mi_math.h"

#include <algorithm>

namespace {
    constexpr float kMinimumHoldDuration = 0.0001f;
}

void BlinkerBehavior::Start()
{
    m_model = GetOwner()->GetComponent<ModelComponent>();
    m_spriteRenderer = GetOwner()->GetComponent<SpriteRendererComponent>();

    RefreshTargets();
    RestoreDefaultStates();
}

void BlinkerBehavior::Update()
{
    if (m_mode == BlinkMode::None) return;

    const bool wasRunning = !m_emissionRateTask.IsFinished();
    m_emissionRateTask.Update(FPS_GetUnscaledDeltaTime());

    switch (m_mode) {
    case BlinkMode::Flash:
        ApplyFlash(m_emissionRateTask.m_currentValue);
        break;
    case BlinkMode::FlashTemporary:
        ApplyFlash(m_emissionRateTask.m_currentValue);
        break;
    case BlinkMode::Reset:
        ApplyReset(m_emissionRateTask.m_currentValue);
        break;
    default:
        break;
    }

    if (wasRunning && m_emissionRateTask.IsFinished()) {
        m_mode = BlinkMode::None;
    }
}

void BlinkerBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Blinker")) {
        ImGui::Text("Target Count: %d", static_cast<int>(m_targets.size()));
        ImGui::Text("Flashing: %s", IsFlashing() ? "true" : "false");

        if (ImGui::Button("Flash Test")) {
            FlashTemporary({ 1.0f, 0.2f, 0.1f }, 3.0f, 0.08f, 0.05f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Blink")) {
            Reset(0.08f);
        }
    }

    InspectorViewWindow::EndComponentSection();
}

void BlinkerBehavior::Flash(const XMFLOAT3& color, float intensity, float duration)
{
    RefreshTargets();
    CaptureStartStates();

    m_emissionRateTask.Reset();
    m_mode = BlinkMode::Flash;
    m_flashColor = color;
    m_flashIntensity = intensity;

    if (duration <= 0.0f) {
        ApplyFlash(1.0f);
        m_mode = BlinkMode::None;
        return;
    }

    m_emissionRateTask.m_startValue = 0.0f;
    m_emissionRateTask.m_targetValue = 1.0f;
    m_emissionRateTask.m_endValue = 1.0f;
    m_emissionRateTask.m_duration = duration;
    m_emissionRateTask.m_holdDuration = 0.0f;
    m_emissionRateTask.Start();
}

void BlinkerBehavior::FlashTemporary(const XMFLOAT3& color, float intensity, float duration, float holdDuration)
{
    RefreshTargets();
    CaptureStartStates();

    m_emissionRateTask.Reset();
    m_mode = BlinkMode::FlashTemporary;
    m_flashColor = color;
    m_flashIntensity = intensity;

    if (duration <= 0.0f && holdDuration <= 0.0f) {
        m_mode = BlinkMode::None;
        return;
    }

    m_emissionRateTask.m_startValue = 0.0f;
    m_emissionRateTask.m_targetValue = 1.0f;
    m_emissionRateTask.m_endValue = 0.0f;
    m_emissionRateTask.m_duration = duration;
    m_emissionRateTask.m_holdDuration = (std::max)(holdDuration, kMinimumHoldDuration);
    m_emissionRateTask.Start();
}

void BlinkerBehavior::Reset(float duration)
{
    RefreshTargets();
    CaptureStartStates();

    m_emissionRateTask.Reset();
    m_mode = BlinkMode::Reset;

    if (duration <= 0.0f) {
        m_mode = BlinkMode::None;
        return;
    }

    m_emissionRateTask.m_startValue = 0.0f;
    m_emissionRateTask.m_targetValue = 1.0f;
    m_emissionRateTask.m_endValue = 1.0f;
    m_emissionRateTask.m_duration = duration;
    m_emissionRateTask.m_holdDuration = 0.0f;
    m_emissionRateTask.Start();
}

void BlinkerBehavior::RefreshTargets()
{
    m_targets.clear();

    if (m_model) {
        auto& materialSlots = m_model->GetMaterialSlots();
        for (size_t i = 0; i < materialSlots.size(); ++i) {
            MaterialTarget target;
            target.type = MaterialTargetType::Model;
            target.slotIndex = i;
            target.defaultState = GetEmissionState(&materialSlots[i]);
            target.startState = target.defaultState;
            m_targets.push_back(target);
        }
    }

    if (m_spriteRenderer) {
        MaterialTarget target;
        target.type = MaterialTargetType::Sprite;
        target.defaultState = GetEmissionState(&m_spriteRenderer->GetMaterial());
        target.startState = target.defaultState;
        m_targets.push_back(target);
    }
}

void BlinkerBehavior::CaptureStartStates()
{
    for (MaterialTarget& target : m_targets) {
        target.startState = GetEmissionState(ResolveMaterial(target));
    }
}

MaterialInstance* BlinkerBehavior::ResolveMaterial(const MaterialTarget& target)
{
    switch (target.type) {
    case MaterialTargetType::Model:
        if (!m_model) return nullptr;
        if (target.slotIndex >= m_model->GetMaterialSlots().size()) return nullptr;
        return &m_model->GetMaterialSlots()[target.slotIndex];
    case MaterialTargetType::Sprite:
        if (!m_spriteRenderer) return nullptr;
        return &m_spriteRenderer->GetMaterial();
    default:
        return nullptr;
    }
}

BlinkerBehavior::EmissionState BlinkerBehavior::GetEmissionState(MaterialInstance* material) const
{
    EmissionState state;
    if (!material) return state;

    state.isOverride = material->isOverrideEmissive;
    state.color = material->overrideEmissiveColor;
    state.intensity = material->isOverrideEmissive ? material->overrideEmissiveIntensity : 0.0f;
    return state;
}

void BlinkerBehavior::ApplyEmissionState(MaterialInstance* material, const EmissionState& state)
{
    if (!material) return;

    material->isOverrideEmissive = state.isOverride;
    material->overrideEmissiveColor = state.color;
    material->overrideEmissiveIntensity = state.intensity;
}

void BlinkerBehavior::ApplyFlash(float rate)
{
    rate = MiMath::Clamp(rate, 0.0f, 1.0f);

    for (MaterialTarget& target : m_targets) {
        MaterialInstance* material = ResolveMaterial(target);
        if (!material) continue;

        EmissionState state;
        state.isOverride = true;
        state.color = MiMath::Lerp(target.startState.color, m_flashColor, rate);
        state.intensity = MiMath::Lerp(target.startState.intensity, m_flashIntensity, rate);

        ApplyEmissionState(material, state);
    }
}

void BlinkerBehavior::ApplyReset(float rate)
{
    rate = MiMath::Clamp(rate, 0.0f, 1.0f);

    for (MaterialTarget& target : m_targets) {
        MaterialInstance* material = ResolveMaterial(target);
        if (!material) continue;

        EmissionState state;
        state.isOverride = false;
        state.color = MiMath::Lerp(target.startState.color, target.defaultState.color, rate);
        state.intensity = MiMath::Lerp(
            target.startState.intensity,
            target.defaultState.isOverride ? target.defaultState.intensity : 0.0f,
            rate);

        ApplyEmissionState(material, state);
    }
}

void BlinkerBehavior::RestoreDefaultStates()
{
    for (MaterialTarget& target : m_targets) {
        ApplyEmissionState(ResolveMaterial(target), target.defaultState);
    }
}
