//===================================================
// hit_stop_behavior.cpp
// 
// Author・哺iu Kitamura
// Date  ・・026/06/06
//===================================================
#include "hit_stop_behavior.h"

#include "Engine/Device/mi_fps.h"
#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

void HitStopBehavior::Start()
{
    m_hitStopTask.Reset();
}

void HitStopBehavior::Update()
{
    m_hitStopTask.Update(FPS_GetUnscaledDeltaTime());
}

void HitStopBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Hit Stop")) {
        ImGui::Text("Running: %s", IsHitStopping() ? "true" : "false");
    }

    InspectorViewWindow::EndComponentSection();
}

void HitStopBehavior::StartHitStop(
    float duration,
    HitStopCallback onEnter,
    HitStopCallback onEntered,
    HitStopCallback onUpdate,
    HitStopCallback onExit)
{
    m_hitStopTask.Reset();

    m_hitStopTask.m_duration = duration;
    m_hitStopTask.m_onEnter = onEnter;
    m_hitStopTask.m_onEntered = onEntered;
    m_hitStopTask.m_onUpdate = onUpdate;
    m_hitStopTask.m_onExit = onExit;

    m_hitStopTask.Start();
}

bool HitStopBehavior::IsHitStopping() const
{
    return m_hitStopTask.IsRunning();
}

void HitStopBehavior::HitStopTask::Start()
{
    SequenceTask::Start();

    if (m_onEnter) {
        m_onEnter();
    }
}

void HitStopBehavior::HitStopTask::Update(float deltaTime)
{
    if (IsFinished()) return;

    SequenceTask::Update(deltaTime);

    switch (m_taskStep) {
    case 0:
        AdvanceStep();
        break;

    case 1:
        if (m_onEntered) {
            m_onEntered();
        }
        AdvanceStep();
        break;

    case 2:
        if (m_onUpdate) {
            m_onUpdate();
        }

        if (m_taskTimer >= m_duration) {
            if (m_onExit) {
                m_onExit();
            }

            Finish();
        }
        break;

    default:
        Finish();
        break;
    }
}
