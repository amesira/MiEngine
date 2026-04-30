//---------------------------------------------------
// game_world.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef GAME_WORLD_H
#define GAME_WORLD_H
#include "./Manager/scene_manager.h"

#include "Engine/Framework/Processor/physics_processor.h"
#include "Engine/Framework/Processor/behavior_processor.h"
#include "Engine/Framework/Processor/camera_processor.h"
#include "Engine/Framework/Processor/render_processor.h"
#include "Engine/Framework/Processor/animation_processor.h"

#include <vector>
#include "Engine/render_view.h"

class GameWorld {
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

    // RenderViewインデックス
    int m_mainSceneRenderViewIndex = 0; // メインシーン用RenderViewのインデックス
    int m_mainGameRenderViewIndex = 1;  // メインゲーム用RenderViewのインデックス
    int m_canvasRenderViewIndex = 2;    // UI描画用RenderViewのインデックス

public:
    void Initialize();
    void Finalize();
    void Update();
    void Render();

    // SceneManagerへのアクセス
    SceneManager& GetSceneManager() { return m_sceneManager; }
    // RenderViewへのアクセス
    std::vector<RenderView>& GetRenderViews() { return m_renderViews; }
    RenderView& GetMainSceneRenderView() { return m_renderViews[m_mainSceneRenderViewIndex]; }
    RenderView& GetMainGameRenderView() { return m_renderViews[m_mainGameRenderViewIndex]; }
    RenderView& GetCanvasRenderView() { return m_renderViews[m_canvasRenderViewIndex]; }

private:
    // SceneRenderViewの設定
    void SetSceneRenderView(IScene* scene, int sceneRenderViewIndex);

};

#endif // GAME_WORLD_H