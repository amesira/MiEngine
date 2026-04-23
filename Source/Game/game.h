//----------------------------------------------------
// game_scene.h [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//----------------------------------------------------
#ifndef GAME_H
#define GAME_H
#include "Engine/Core/scene_base.h"

class TextComponent;
class ButtonBehavior;
class TpsCameraBehavior;
class ImageComponent;

// ゲームシーン
class GameScene : public SceneBase {
private:
    int m_level = 1;

    bool m_isPlaying = false;

    TextComponent* m_startText = nullptr;
    float m_startTimer = 0.0f;

    ButtonBehavior* m_backTitleButton = nullptr;
    ButtonBehavior* m_helpButton = nullptr;

    TpsCameraBehavior* m_tpsCamera = nullptr;

    int m_tutorialStep = 0;
    std::vector<GameObject*> m_tutorialPages;
    ImageComponent* m_tutorialImage = nullptr;
    ButtonBehavior* m_tutorialLeftButton = nullptr;
    ButtonBehavior* m_tutorialRightButton = nullptr;

    int m_bgmHandle = -1;

public:
    void    Initialize() override;
    void    Finalize() override;

    void    Update() override;
    void    Draw() override;

    void SetGameLevel(int level) { m_level = level; }
};

#endif