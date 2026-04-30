//===================================================
// scene_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/30
//===================================================
#include "game_view_window.h"
#include "Engine/Editor/editor_context.h"
#include "Engine/render_view.h"

void GameViewWindow::Draw()
{
    const RenderView* renderView = m_editorContext->gameRenderView;

    ID3D11ShaderResourceView* srv = renderView->colorBufferSRV.Get();
    float aspectRatio = renderView->aspectRatio;

    ImVec2 avail = ImGui::GetContentRegionAvail();
    if (avail.x / avail.y > aspectRatio) {
        avail.x = avail.y * aspectRatio;
    }
    else if (avail.x / avail.y < aspectRatio) {
        avail.y = avail.x / aspectRatio;
    }

    ImGui::Image((ImTextureID)(srv), avail);
}
