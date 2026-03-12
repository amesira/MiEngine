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
    // sceneViewの大きさを計算（16:9のアスペクト比を維持）
    float sceneWidth = m_editorContext->displayX - m_editorContext->hierarchyWidth - m_editorContext->inspectorWidth;
    float sceneHeight = sceneWidth * 9.0f / 16.0f; // 16:9のアスペクト比を維持

    // Debug View ウィンドウの位置とサイズを設定
    ImGui::SetNextWindowPos({ 0.0f, m_editorContext->toolbarHeight + sceneHeight });
    float width = sceneWidth;
    float height = m_editorContext->displayY - (m_editorContext->toolbarHeight + sceneHeight);
    ImGui::SetNextWindowSize({ width, height });

    // Debug View ウィンドウの描画
    ImGui::Begin("Debug View", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse);

    {
        if (ImGui::Button("Clear")) {
            m_editorContext->logMessages.clear();
        }

        ImGui::Separator();

        ImGui::BeginChild("LogRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        bool isScrollToBottom =
            (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f);

        for (const std::string& message : m_editorContext->logMessages) {
            ImGui::TextWrapped("%s", message.c_str());
        }

        if (isScrollToBottom) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
    }

    ImGui::End();
}
