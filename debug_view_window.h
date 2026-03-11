//---------------------------------------------------
// debug_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef DEBUG_VIEW_WINDOW_H
#define DEBUG_VIEW_WINDOW_H
#include "imgui_window_interface.h"
class EditorContext;

class DebugViewWindow : public IImguiWindow {
private:
    EditorContext* m_editorContext;

public:
    DebugViewWindow(EditorContext* editorContext) : m_editorContext(editorContext) {}
    void Draw() override;
};

#endif // DEBUG_VIEW_WINDOW_H