//---------------------------------------------------
// editor_manager.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/30
//---------------------------------------------------
#ifndef EDITOR_MANAGER_H
#define EDITOR_MANAGER_H

#include "mi_imgui_manager.h"
#include "editor_context.h"

#include "./EditorWindow/hierarchy_view_window.h"
#include "./EditorWindow/inspector_view_window.h"
#include "./EditorWindow/scene_view_window.h"
#include "./EditorWindow/tool_bar_window.h"
#include "./EditorWindow/debug_view_window.h"

class EditorManager {
private:
    MiImguiManager  m_imguiManager;     // ImGuiウィンドウの管理
    EditorContext   m_editorContext;    // エディタ全体の状態管理

    HierarchyViewWindow m_hierarchyViewWindow;  // GameObjectの階層表示ウィンドウ
    InspectorViewWindow m_inspectorViewWindow;  // 選択GameObjectの詳細表示ウィンドウ
    SceneViewWindow     m_sceneViewWindow;      // シーンの3D表示ウィンドウ
    ToolBarWindow       m_toolBarWindow;        // ツールバーウィンドウ（上部のメニューやボタンなど）
    DebugViewWindow     m_debugViewWindow;      // デバッグ情報表示ウィンドウ

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