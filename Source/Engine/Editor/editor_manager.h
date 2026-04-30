//---------------------------------------------------
// editor_manager.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/30
//---------------------------------------------------
#ifndef EDITOR_MANAGER_H
#define EDITOR_MANAGER_H
#include <array>
#include <vector>
#include <string>

#include "mi_imgui_manager.h"
#include "editor_context.h"

#include "./EditorWindow/hierarchy_view_window.h"
#include "./EditorWindow/inspector_view_window.h"
#include "./EditorWindow/scene_view_window.h"
#include "./EditorWindow/tool_bar_window.h"
#include "./EditorWindow/debug_view_window.h"

// タブエリアの構造体
struct EditorTabArea {
    // タブが１つの場合はタブ表示を省略して直接ウィンドウを表示する
    bool singleTabMode = false;

    // エリアの名前
    std::string areaName = "Default";

    // x, y, width, height（スクリーンサイズを基準とした割合で指定）
    std::array<float, 4> areaRect = { 0.0f, 0.0f, 1.0f, 1.0f };

    // タブの状態管理
    int activeTabIndex = 0;         // 現在アクティブなタブのインデックス
    std::vector<IImguiWindow*> tabs;// タブに表示するウィンドウのリスト
    std::vector<std::string> tabNames;   // タブの名前リスト（UI表示用）

    void AddWindow(IImguiWindow* tab, std::string tabName) {
        tabs.push_back(tab);
        tabNames.push_back(tabName);
    }
};

// エディタ内の主要なエリアID
enum class EditorAreaID {
    CenterScreen = 0,   // 中央のメインエリア
    Bottom,             // 下部のエリア（ログやコンソールなど）
    Right01,            // 右側のエリア1（Hierarchyなど）
    Right02,            // 右側のエリア2（Inspector、Settingsなど）
    Top,                // 上部のエリア（ツールバーなど）
    MAX,
};

// エディタ全体の管理クラス
class EditorManager {
private:
    MiImguiManager  m_imguiManager;     // ImGuiウィンドウの管理
    EditorContext   m_editorContext;    // エディタ全体の状態管理

    HierarchyViewWindow m_hierarchyViewWindow;  // GameObjectの階層表示ウィンドウ
    InspectorViewWindow m_inspectorViewWindow;  // 選択GameObjectの詳細表示ウィンドウ
    SceneViewWindow     m_sceneViewWindow;      // シーンの3D表示ウィンドウ
    ToolBarWindow       m_toolBarWindow;        // ツールバーウィンドウ（上部のメニューやボタンなど）
    DebugViewWindow     m_debugViewWindow;      // デバッグ情報表示ウィンドウ

    EditorTabArea m_tabAreas[static_cast<int>(EditorAreaID::MAX)]; // 各エリアのタブ管理

public:
    EditorManager()
        : m_hierarchyViewWindow(&m_editorContext)
        , m_inspectorViewWindow(&m_editorContext)
        , m_sceneViewWindow(&m_editorContext)
        , m_toolBarWindow(&m_editorContext)
        , m_debugViewWindow(&m_editorContext)
    {
    }
    ~EditorManager() = default;

    void Initialize(HWND hWnd);
    void Finalize();
    void Render();

    // EditorContextへのアクセス
    EditorContext& GetEditorContext() { return m_editorContext; }

};

#endif // EDITOR_MANAGER_H