//===================================================
// inspector_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "inspector_view_window.h"
#include "engine.h"

#include "editor_context.h"
#include "render_view.h"
#include "game_object.h"

void InspectorViewWindow::Draw()
{
    ImGui::SetNextWindowPos({ m_editorContext->displayX - m_editorContext->inspectorWidth , m_editorContext->toolbarHeight });
    float width = m_editorContext->inspectorWidth;
    float height = m_editorContext->displayY - m_editorContext->toolbarHeight;
    ImGui::SetNextWindowSize({ width, height });

    ImGui::Begin("Inspector View", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Selected Object: %s", m_editorContext->selectedObject ? m_editorContext->selectedObject->GetName().c_str() : "None");

    ImGui::End();
}
