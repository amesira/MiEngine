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
    float height = width * 9.0f / 16.0f;
    ImGui::SetNextWindowSize({ width, height });

    ImGui::Begin("ViewPort", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoTitleBar);

    if (ImGui::BeginTabBar("ViewTabs")) {
        ImVec2 avail = ImGui::GetContentRegionAvail();

        if (ImGui::BeginTabItem("SceneView")) {
            const RenderView* renderView = m_editorContext->sceneRenderView;
            ImGui::Image((ImTextureID)(renderView->colorBufferSRV.Get()), avail);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("GameView")) {
            const RenderView* renderView = m_editorContext->sceneRenderView;
            ImGui::Image((ImTextureID)(renderView->depthBufferSRV.Get()), avail);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}