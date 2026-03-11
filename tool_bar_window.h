//---------------------------------------------------
// tool_bar_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef TOOL_BAR_WINDOW_H
#define TOOL_BAR_WINDOW_H
#include "imgui_window_interface.h"
class EditorContext;

class ToolBarWindow : public IImguiWindow {
private:
    EditorContext* m_editorContext;

public:
    ToolBarWindow(EditorContext* editorContext) : m_editorContext(editorContext) {}
    void Draw() override;
};

#endif // TOOL_BAR_WINDOW_H