//---------------------------------------------------
// game_world.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef GAME_WORLD_H
#define GAME_WORLD_H
#include "scene_manager.h"

#include "Engine/Framework/Processor/physics_processor.h"
#include "Engine/Framework/Processor/behavior_processor.h"
#include "Engine/Framework/Processor/camera_processor.h"
#include "Engine/Framework/Processor/render_processor.h"
#include "Engine/Framework/Processor/animation_processor.h"

#include <vector>
#include "Engine/render_view.h"

class GameWorld {
public:
    void Initialize();
    void Finalize();
    void Update();
    void Render();

private:
    // Scene管理
    SceneManager    m_sceneManager;

    // Processor群
    PhysicsProcessor   m_physicsProcessor;
    AnimationProcessor m_animationProcessor;
    BehaviorProcessor  m_behaviorProcessor;

    std::vector<RenderView> m_renderViews;
    CameraProcessor    m_cameraProcessor;
    RenderProcessor    m_renderProcessor;

public:
    // SceneManagerへのアクセス
    SceneManager& GetSceneManager() { return m_sceneManager; }
    // RenderViewへのアクセス
    std::vector<RenderView>& GetRenderViews() { return m_renderViews; }

};

#endif // GAME_WORLD_H