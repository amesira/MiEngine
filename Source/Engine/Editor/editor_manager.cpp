//===================================================
// editor_manager.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/30
//===================================================
#include "editor_manager.h"

void EditorManager::Initialize(HWND hWnd)
{
    m_imguiManager.Initialize(hWnd);
}

void EditorManager::Finalize()
{
    m_imguiManager.Finalize();
}

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