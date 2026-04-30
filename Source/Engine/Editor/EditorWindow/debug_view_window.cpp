//===================================================
// debug_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "debug_view_window.h"

#include "Engine/engine.h"
#include "Engine/Editor/editor_context.h"

#include "Engine/render_view.h"
#include "Engine/Core/game_object.h"

void DebugViewWindow::Draw()
{
    // ログのクリアボタン
    if (ImGui::Button("Clear")) {
        m_editorContext->logMessages.clear();
    }

    ImGui::Separator();

    // ログ表示エリア
    ImGui::BeginChild("LogRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    bool isScrollToBottom =
        (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f);

    // ログメッセージを表示
    for (const std::string& message : m_editorContext->logMessages) {
        ImGui::TextWrapped("%s", message.c_str());
    }

    if (isScrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}
