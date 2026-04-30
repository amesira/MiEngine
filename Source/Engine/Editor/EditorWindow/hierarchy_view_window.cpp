//===================================================
// hierarchy_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "hierarchy_view_window.h"

#include "Engine/engine.h"
#include "Engine/Editor/editor_context.h"
#include "Engine/render_view.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"

void HierarchyViewWindow::Draw()
{
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
}
