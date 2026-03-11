//----------------------------------------------------
// mi_imgui_manager.cpp [imguiマネージャー]
// 
// Author：Miu Kitamura
// Date  ：2025/07/01
//----------------------------------------------------
#ifndef MI_IMGUI_MANAGER_H
#define MI_IMGUI_MANAGER_H
#include "imgui_window_interface.h"
#include <vector>
#include <memory>
#include <windows.h>

class MiImguiManager {
    std::vector<std::unique_ptr<IImguiWindow>> m_imguiWindowList;

public:
    void Initialize(HWND hwnd);
    void Finalize();

    void RenderProcess();

    void AddWindow(std::unique_ptr<IImguiWindow> window) {
        m_imguiWindowList.push_back(std::move(window));
    }

private:
    void SetupEditorImguiStyle();

};

#endif