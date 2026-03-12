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

void ToolBarWindow::Draw()
{
    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize({ m_editorContext->displayX, m_editorContext->toolbarHeight });

    ImGui::Begin("Toolbar", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Mi Engine v1");

    ImGui::End();
}
