//===================================================
// tool_bar_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "tool_bar_window.h"
#include "Engine/engine.h"

#include "Engine/Editor/editor_context.h"
#include "Engine/render_view.h"
#include "Engine/Core/game_object.h"

#include "Engine/System/Device/mi_fps.h"

void ToolBarWindow::Draw()
{
    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize({ m_editorContext->displayX, m_editorContext->toolbarHeight });

    ImGui::Begin("Toolbar", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));

    ImGui::Text("Mi Engine v1 : ");
    ImGui::SameLine();

    ImGui::Text("FPS: %.1f", FPS_GetFPS());
    ImGui::SameLine();

    const char* items[] = { "Edit", "Play" };
    int currentIndex = static_cast<int>(m_editorContext->currentEditorMode);

    ImGui::SetNextItemWidth(120);
    if (ImGui::Combo("##Mode", &currentIndex, items, IM_ARRAYSIZE(items))) {
        // currentIndex が変わったとき
        EditorContext::EditorMode newMode = static_cast<EditorContext::EditorMode>(currentIndex);
        m_editorContext->currentEditorMode = newMode;
    }
    ImGui::SameLine();

    if (ImGui::Button("Reload Scene")) {
        m_editorContext->triggerSceneReload = true;
    }

    ImGui::PopStyleVar();

    ImGui::End();
}
