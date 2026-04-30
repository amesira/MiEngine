//===================================================
// editor_manager.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/30
//===================================================
#include "editor_manager.h"

// EditorManagerの初期化
void EditorManager::Initialize(HWND hWnd)
{
    m_imguiManager.Initialize(hWnd);

    // EditorContextの初期化
    m_editorContext.displayX = 1920.0f;
    m_editorContext.displayY = 1080.0f;

    // Tabエリアの初期化
    EditorTabArea& centerArea = m_tabAreas[static_cast<int>(EditorAreaID::CenterScreen)];
    centerArea.areaName = "CenterArea";
    centerArea.areaRect = { 0.0f, 0.04f, 0.6f, 0.6f };
    centerArea.AddWindow(&m_sceneViewWindow, "Scene");
    centerArea.AddWindow(&m_gameViewWindow, "Game");
    centerArea.AddWindow(&m_canvasViewWindow, "Canvas");

    EditorTabArea& rightArea1 = m_tabAreas[static_cast<int>(EditorAreaID::Right01)];
    rightArea1.areaName = "Right1";
    rightArea1.areaRect = { 0.6f, 0.04f, 0.2f, 0.96f };
    rightArea1.AddWindow(&m_hierarchyViewWindow, "Hierarchy");

    EditorTabArea& rightArea2 = m_tabAreas[static_cast<int>(EditorAreaID::Right02)];
    rightArea2.areaName = "Right2";
    rightArea2.areaRect = { 0.8f, 0.04f, 0.2f, 0.96f };
    rightArea2.AddWindow(&m_inspectorViewWindow, "Inspector");
    rightArea2.AddWindow(&m_settingsViewWindow, "Settings");

    EditorTabArea& bottomArea = m_tabAreas[static_cast<int>(EditorAreaID::Bottom)];
    bottomArea.areaName = "Bottom";
    bottomArea.areaRect = { 0.0f, 0.64f, 0.6f, 0.36f };
    bottomArea.AddWindow(&m_debugViewWindow, "Debug");
    bottomArea.AddWindow(&m_sceneViewWindow, "Scene Mini");
    bottomArea.AddWindow(&m_gameViewWindow, "Game Mini");

    EditorTabArea& topArea = m_tabAreas[static_cast<int>(EditorAreaID::Top)];
    topArea.singleTabMode = true;
    topArea.areaName = "Top";
    topArea.areaRect = { 0.0f, 0.0f, 1.0f, 0.04f };
    topArea.AddWindow(&m_toolBarWindow, "Toolbar");

}

// EditorManagerの終了処理
void EditorManager::Finalize()
{
    m_imguiManager.Finalize();
}

// EditorManagerの描画処理
void EditorManager::Render()
{
    m_imguiManager.BeginFrame();

    // 各エリアのタブを描画
    for (int i = 0; i < static_cast<int>(EditorAreaID::MAX); ++i) {
        EditorTabArea& area = m_tabAreas[i];
        if (area.tabs.empty()) continue;

        // タブエリアの位置とサイズを設定
        ImGui::SetNextWindowPos({ m_editorContext.displayX * area.areaRect[0], m_editorContext.displayY * area.areaRect[1] });
        ImGui::SetNextWindowSize({ m_editorContext.displayX * area.areaRect[2], m_editorContext.displayY * area.areaRect[3]});

        // タブエリアの描画
        ImGui::Begin(area.areaName.c_str(), nullptr,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        // タブが1つだけの場合はタブ表示を省略して直接ウィンドウを描画
        if (area.singleTabMode) {
            IImguiWindow* window = area.tabs[0];
            if (window) {
                window->Draw();
            }
            ImGui::End();
            continue;
        }

        // タブの描画
        if (ImGui::BeginTabBar(area.areaName.c_str())) {
            ImVec2 avail = ImGui::GetContentRegionAvail();

            for (size_t tabIndex = 0; tabIndex < area.tabs.size(); tabIndex++) {
                if (ImGui::BeginTabItem(area.tabNames[tabIndex].c_str())) {
                    IImguiWindow* window = area.tabs[tabIndex];
                    if (window) {
                        ImGui::BeginChild(area.tabNames[tabIndex].c_str(), avail, false);
                        window->Draw();
                        ImGui::EndChild();
                    }
                    ImGui::EndTabItem();
                }
            }
        }
        ImGui::EndTabBar();

        ImGui::End();
    }

    m_imguiManager.EndFrame();
}