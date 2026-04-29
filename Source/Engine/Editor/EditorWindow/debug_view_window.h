//---------------------------------------------------
// debug_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef DEBUG_VIEW_WINDOW_H
#define DEBUG_VIEW_WINDOW_H
#include "imgui_window_interface.h"

class DebugViewWindow : public IImguiWindow {
public:
    DebugViewWindow(EditorContext* editorContext) : IImguiWindow(editorContext) {}
    void Draw() override;
};

#endif // DEBUG_VIEW_WINDOW_H