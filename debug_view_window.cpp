//===================================================
// debug_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "debug_view_window.h"
#include "engine.h"

#include "editor_context.h"
#include "render_view.h"
#include "game_object.h"

void DebugViewWindow::Draw()
{
    float sceneWidth = m_editorContext->displayX - m_editorContext->hierarchyWidth - m_editorContext->inspectorWidth;
    float sceneHeight = sceneWidth * 9.0f / 16.0f; // 16:9のアスペクト比を維持

    ImGui::SetNextWindowPos({ 0.0f, m_editorContext->toolbarHeight + sceneHeight });
    float width = sceneWidth;
    float height = m_editorContext->displayY - (m_editorContext->toolbarHeight + sceneHeight);
    ImGui::SetNextWindowSize({ width, height });

    ImGui::Begin("Debug View", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse);

    ImGui::End();
}
