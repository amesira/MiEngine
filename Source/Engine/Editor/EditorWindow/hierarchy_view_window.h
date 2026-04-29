//---------------------------------------------------
// hierarchy_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef HIERARCHY_VIEW_WINDOW_H
#define HIERARCHY_VIEW_WINDOW_H
#include "imgui_window_interface.h"

class HierarchyViewWindow : public IImguiWindow {
public:
    HierarchyViewWindow(EditorContext* editorContext) : IImguiWindow(editorContext) {}
    void Draw() override;
};

#endif // HIERARCHY_VIEW_WINDOW_H