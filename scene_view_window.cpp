//===================================================
// scene_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "scene_view_window.h"
#include "engine.h"

#include "editor_context.h"
#include "render_view.h"

void SceneViewWindow::Draw()
{
    ImGui::SetNextWindowPos({ 0.0f, m_editorContext->toolbarHeight });
    float width = m_editorContext->displayX - m_editorContext->hierarchyWidth - m_editorContext->inspectorWidth;
    float height = width * 9.0f / 16.0f; // 16:9のアスペクト比を維持
    ImGui::SetNextWindowSize({width, height });

    ImGui::Begin("Scene View", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse);

    // ここにシーンビューの内容を描画するコードを追加
    const RenderView* renderView = m_editorContext->sceneRenderView;
    ImTextureID sceneTextureID = (ImTextureID)(renderView->colorBufferSRV.Get());

    ImVec2 size = ImGui::GetContentRegionAvail(); // ウィンドウの利用可能なサイズを取得
    ImGui::Image(sceneTextureID, size); // ダミーの画像表示（実際にはシーンのレンダリング結果をテクスチャとして表示する���

    ImGui::End();
}