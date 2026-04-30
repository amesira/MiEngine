//---------------------------------------------------
// canvas_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef CANVAS_VIEW_WINDOW_H
#define CANVAS_VIEW_WINDOW_H
#include "imgui_window_interface.h"

class CanvasViewWindow : public IImguiWindow {
public:
    CanvasViewWindow(EditorContext* editorContext) : IImguiWindow(editorContext) {}
    void Draw() override;
};

#endif // CANVAS_VIEW_WINDOW_H