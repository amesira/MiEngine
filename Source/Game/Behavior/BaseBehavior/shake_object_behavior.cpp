//===================================================
// shake_object_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/07
//===================================================
#include "shake_object_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"
#include "Engine/Framework/Component/transform_component.h"
#include "Utility/mi_math.h"
#include "Utility/mi_signal.h"

#include <algorithm>

void ShakeObjectBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
}

void ShakeObjectBehavior::Update()
{
    if (!m_transform) return;
    if (!IsShaking()) return;

    const XMFLOAT3 basePosition = GetBasePosition();

    if (!m_shakeTask.IsFinished()) {
        m_shakeTask.Update(FPS_GetUnscaledDeltaTime());
        ApplyOffset(basePosition, m_shakeTask.m_currentOffset);

        if (m_shakeTask.IsFinished()) {
            ClearOffset();
        }
        return;
    }

    if (!m_resetOffsetTask.IsFinished()) {
        m_resetOffsetTask.Update(FPS_GetUnscaledDeltaTime());
        ApplyOffset(basePosition, m_resetOffsetTask.m_currentOffset);

        if (m_resetOffsetTask.IsFinished()) {
            ClearOffset();
        }
    }
}

void ShakeObjectBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Shake Object")) {
        ImGui::Text("Shaking: %s", IsShaking() ? "true" : "false");

        if (ImGui::Button("Shake Test")) {
            Shake(0.25f, 0.12f, 35.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Shake")) {
            Reset(0.08f);
        }
    }

    InspectorViewWindow::EndComponentSection();
}

void ShakeObjectBehavior::Shake(float duration, float magnitude, float frequency)
{
    if (!m_transform) return;

    m_resetOffsetTask.Reset();
    m_shakeTask.Reset();

    if (duration <= 0.0f || magnitude <= 0.0f) {
        ClearOffset();
        return;
    }

    m_shakeTask.m_duration = duration;
    m_shakeTask.m_magnitude = magnitude;
    m_shakeTask.m_frequency = frequency;
    m_shakeTask.Start();
}

void ShakeObjectBehavior::ShakeTemporary(float duration, float magnitude, float frequency)
{
    Shake(duration, magnitude, frequency);
}

void ShakeObjectBehavior::Reset(float duration)
{
    if (!m_transform) return;

    m_shakeTask.Reset();
    m_resetOffsetTask.Reset();

    if (duration <= 0.0f) {
        ClearOffset();
        return;
    }

    m_resetOffsetTask.m_duration = duration;
    m_resetOffsetTask.m_startOffset = m_previousOffset;
    m_resetOffsetTask.Start();
}

bool ShakeObjectBehavior::IsShaking()
{
    return !m_shakeTask.IsFinished() || !m_resetOffsetTask.IsFinished();
}

XMFLOAT3 ShakeObjectBehavior::GetBasePosition() const
{
    XMFLOAT3 currentPosition = m_transform->GetPosition();
    if (!m_hasAppliedOffset) return currentPosition;

    return MiMath::Subtract(currentPosition, m_previousOffset);
}

void ShakeObjectBehavior::ApplyOffset(const XMFLOAT3& basePosition, const XMFLOAT3& offset)
{
    m_transform->SetPosition(MiMath::Add(basePosition, offset));
    m_previousOffset = offset;
    m_hasAppliedOffset = true;
}

void ShakeObjectBehavior::ClearOffset()
{
    if (!m_transform) return;

    const XMFLOAT3 basePosition = GetBasePosition();
    m_transform->SetPosition(basePosition);

    m_previousOffset = { 0.0f, 0.0f, 0.0f };
    m_hasAppliedOffset = false;
}

void ShakeObjectBehavior::ShakeTask::Update(float deltaTime)
{
    if (IsFinished()) return;

    SequenceTask::Update(deltaTime);

    float rate = 1.0f;
    if (m_duration > 0.0f) {
        rate = (std::min)(m_taskTimer / m_duration, 1.0f);
    }

    const float fade = 1.0f - rate;
    const float currentMagnitude = m_magnitude * fade * fade;

    m_currentOffset = { 0.0f, 0.0f, 0.0f };
    m_currentOffset.x = MiSignal::Perlin1D((m_taskTimer + 0.0f) * m_frequency) * currentMagnitude;
    m_currentOffset.y = MiSignal::Perlin1D((m_taskTimer + 37.0f) * m_frequency) * currentMagnitude;
    m_currentOffset.z = MiSignal::Perlin1D((m_taskTimer + 91.0f) * m_frequency) * currentMagnitude;

    if (rate >= 1.0f) {
        m_currentOffset = { 0.0f, 0.0f, 0.0f };
        Finish();
    }
}

void ShakeObjectBehavior::ResetOffsetTask::Start()
{
    SequenceTask::Start();
    m_currentOffset = m_startOffset;
}

void ShakeObjectBehavior::ResetOffsetTask::Update(float deltaTime)
{
    if (IsFinished()) return;

    SequenceTask::Update(deltaTime);

    float rate = 1.0f;
    if (m_duration > 0.0f) {
        rate = (std::min)(m_taskTimer / m_duration, 1.0f);
    }

    m_currentOffset = MiMath::Lerp(m_startOffset, { 0.0f, 0.0f, 0.0f }, rate);

    if (rate >= 1.0f) {
        m_currentOffset = { 0.0f, 0.0f, 0.0f };
        Finish();
    }
}
