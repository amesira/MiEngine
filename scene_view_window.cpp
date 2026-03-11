//===================================================
// scene_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "scene_view_window.h"
#include "engine.h"

void SceneViewWindow::Draw()
{
    ImGui::Begin("Scene View");
    ImGui::SetWindowSize(ImVec2(600, 400), ImGuiCond_Once);

    // ここにシーンビューの内容を描画するコードを追加
    ImGui::Text("This is the Scene View window.");
    ImGui::Image(m_sceneTextureID, ImVec2(400, 300)); // ダミーの画像表示（実際にはシーンのレンダリング結果をテクスチャとして表示する���

    ImGui::End();
}