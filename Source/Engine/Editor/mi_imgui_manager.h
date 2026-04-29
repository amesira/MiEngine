//----------------------------------------------------
// mi_imgui_manager.cpp [imguiマネージャー]
// 
// Author：Miu Kitamura
// Date  ：2025/07/01
//----------------------------------------------------
#ifndef MI_IMGUI_MANAGER_H
#define MI_IMGUI_MANAGER_H
#include <windows.h>

class MiImguiManager {
public:
    void Initialize(HWND hwnd);
    void Finalize();
    void BeginFrame();
    void EndFrame();

private:
    void SetupEditorImguiStyle();

};

#endif