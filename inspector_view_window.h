//---------------------------------------------------
// inspector_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef INSPECTOR_VIEW_WINDOW_H
#define INSPECTOR_VIEW_WINDOW_H
#include "imgui_window_interface.h"
class EditorContext;

class InspectorViewWindow : public IImguiWindow {
private:
    EditorContext* m_editorContext;

public:
    InspectorViewWindow(EditorContext* editorContext) : m_editorContext(editorContext) {}
    void Draw() override;
};

#endif // INSPECTOR_VIEW_WINDOW_H