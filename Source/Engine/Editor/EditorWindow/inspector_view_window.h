//---------------------------------------------------
// inspector_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef INSPECTOR_VIEW_WINDOW_H
#define INSPECTOR_VIEW_WINDOW_H
#include "imgui_window_interface.h"
class GameObject;
class Component;

class InspectorViewWindow : public IImguiWindow {
public:
    InspectorViewWindow(EditorContext* editorContext) : IImguiWindow(editorContext) {}
    void Draw() override;

    // Component表示初め
    static bool BeginComponentSection(Component* comp, const char* name, bool useEnableSetting = true);
    static void EndComponentSection();

private:
    void DrawComponentInspector(GameObject* gameObject);

};

#endif // INSPECTOR_VIEW_WINDOW_H