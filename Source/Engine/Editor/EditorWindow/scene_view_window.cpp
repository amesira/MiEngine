//===================================================
// scene_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "scene_view_window.h"
#include "Engine/engine.h"

#include "Engine/Editor/editor_context.h"
#include "Engine/render_view.h"

void SceneViewWindow::Draw()
{
    const RenderView* renderView = m_editorContext->sceneRenderView;
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)(renderView->colorBufferSRV.Get()), avail);
}