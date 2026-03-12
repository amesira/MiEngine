//===================================================
// tool_bar_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "tool_bar_window.h"
#include "engine.h"

#include "editor_context.h"
#include "render_view.h"
#include "game_object.h"

#include "mi_fps.h"

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
    static int currentIndex = 0;

    ImGui::SetNextItemWidth(120);
    if (ImGui::Combo("##Mode", &currentIndex, items, IM_ARRAYSIZE(items))) {
        // currentIndex が変わったとき
        if (currentIndex == 0) {
            m_editorContext->currentEditorMode = EditorContext::EditorMode::Edit;
        }
        else if (currentIndex == 1) {
            m_editorContext->currentEditorMode = EditorContext::EditorMode::Play;
        }
    }
    ImGui::SameLine();

    if (ImGui::Button("Reload Scene")) {
        m_editorContext->triggerSceneReload = true;
    }

    ImGui::PopStyleVar();

    ImGui::End();
}
