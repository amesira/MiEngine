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

#include "scene_interface.h"

void HierarchyViewWindow::Draw()
{
    // Hierarchy View ウィンドウの位置とサイズを設定
    ImGui::SetNextWindowPos({ m_editorContext->displayX - m_editorContext->hierarchyWidth - m_editorContext->inspectorWidth,
        m_editorContext->toolbarHeight });
    float width = m_editorContext->hierarchyWidth;
    float height = m_editorContext->displayY - m_editorContext->toolbarHeight;
    ImGui::SetNextWindowSize({ width, height });

    // Hierarchy View ウィンドウの描画
    ImGui::Begin("Hierarchy View", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse);

    // シーン内のGameObjectをリスト表示
    GameObject* selected = m_editorContext->selectedObject;

    IScene* scene = m_editorContext->scene;
    if (scene){
        auto& gameObjects = scene->GetGameObjects();

        for (GameObject& obj : gameObjects){
            bool isSelected = (selected == &obj);

            // ImGuiのIDスタックにGameObjectのアドレスをプッシュして、同名のGameObjectがあっても区別できるようにする
            ImGui::PushID(&obj);

            // アクティブでないオブジェクトはグレーアウトして表示
            if (!obj.GetActive()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            }

            ImGui::Text("-");
            ImGui::SameLine();
            // 選択状態の表示と選択処理
            if (ImGui::Selectable(obj.GetName().c_str(), isSelected)){
                m_editorContext->selectedObject = &obj;
            }

            // グレーアウトの解除
            if (!obj.GetActive()) {
                ImGui::PopStyleColor();
            }

            ImGui::PopID();
        }
    }

    ImGui::End();
}
