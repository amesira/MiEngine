//---------------------------------------------------
// scene_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef SCENE_VIEW_WINDOW_H
#define SCENE_VIEW_WINDOW_H
#include "imgui_window_interface.h"
class EditorContext;

class SceneViewWindow : public IImguiWindow {
private:
    EditorContext* m_editorContext;

public:
    SceneViewWindow(EditorContext* editorContext) : m_editorContext(editorContext) {}
    void Draw() override;
};

#endif // SCENE_VIEW_WINDOW_H