//---------------------------------------------------
// imgui_window_interface.h [imguiウィンドウ制御クラス]
// 
// imguiウィンドウの管理・制御を行う。
// 
// Author：Miu Kitamura
// Date  ：2025/07/01
//---------------------------------------------------
#ifndef IMGUI_WINDOW_INTERFACE_H
#define IMGUI_WINDOW_INTERFACE_H
#include "imgui.h"
#include "imgui_internal.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

class IImguiWindow {
public:
    IImguiWindow() = default;
    virtual ~IImguiWindow() = default;
    virtual void Draw() = 0;
};

#endif // IMGUI_WINDOW_INTERFACE_H