//---------------------------------------------------
// game_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef GAME_VIEW_WINDOW_H
#define GAME_VIEW_WINDOW_H
#include "imgui_window_interface.h"

class GameViewWindow : public IImguiWindow {
public:
    GameViewWindow(EditorContext* editorContext) : IImguiWindow(editorContext) {}
    void Draw() override;
};

#endif // GAME_VIEW_WINDOW_H