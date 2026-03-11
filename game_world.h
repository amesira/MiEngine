//---------------------------------------------------
// game_world.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef GAME_WORLD_H
#define GAME_WORLD_H
#include "scene_manager.h"

#include "physics_processor.h"
#include "behavior_processor.h"
#include "camera_processor.h"
#include "render_processor.h"

#include <vector>
#include "render_view.h"

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
    BehaviorProcessor  m_behaviorProcessor;

    std::vector<RenderView> m_renderViews;
    CameraProcessor    m_cameraProcessor;
    RenderProcessor    m_renderProcessor;
};

#endif // GAME_WORLD_H