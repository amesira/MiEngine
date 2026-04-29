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

    m_hierarchyViewWindow.Draw();
    m_inspectorViewWindow.Draw();
    m_sceneViewWindow.Draw();
    m_toolBarWindow.Draw();
    m_debugViewWindow.Draw();

    m_imguiManager.EndFrame();
}