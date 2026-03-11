//===================================================
// hierarchy_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "hierarchy_view_window.h"
#include "engine.h"

#include "editor_context.h"
#include "render_view.h"
#include "game_object.h"

void HierarchyViewWindow::Draw()
{
    ImGui::SetNextWindowPos({ m_editorContext->displayX - m_editorContext->hierarchyWidth - m_editorContext->inspectorWidth,
        m_editorContext->toolbarHeight });
    float width = m_editorContext->hierarchyWidth;
    float height = m_editorContext->displayY - m_editorContext->toolbarHeight;
    ImGui::SetNextWindowSize({ width, height });

    ImGui::Begin("Hierarchy View", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse);

   // ImGui::SetNextItemOpen(true, ImGuiCond_Once);

    ImGui::End();
}
