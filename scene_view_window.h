//---------------------------------------------------
// scene_view_window.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//---------------------------------------------------
#ifndef SCENE_VIEW_WINDOW_H
#define SCENE_VIEW_WINDOW_H
#include "imgui_window_interface.h"

class SceneViewWindow : public IImguiWindow {
private:
    ImTextureID m_sceneTextureID; // シーンのレンダリング結果を格納するテクスチャID

public:
    void Initialize(ImTextureID sceneTextureID) {
        m_sceneTextureID = sceneTextureID;
    }
    void Draw() override;
};

#endif // SCENE_VIEW_WINDOW_H