//===================================================
// game_world.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "game_world.h"
#include "debug_renderer.h"
#include "engine_service_locator.h"

// GameWorldの初期化
void GameWorld::Initialize()
{
    // Scene管理の初期化
    m_sceneManager.Initialize();

    // Processor群の初期化
    m_physicsProcessor.Initialize();
    m_animationProcessor.Initialize();
    m_behaviorProcessor.Initialize();
    m_cameraProcessor.Initialize();
    m_renderProcessor.Initialize();

    // RenderViewの初期化
    m_renderViews.resize(8);
    for (RenderView& view : m_renderViews) {
        view.Initialize();
    }
}

// GameWorldの終了処理
void GameWorld::Finalize()
{
    // Processor群の終了処理
    m_physicsProcessor.Finalize();
    m_animationProcessor.Finalize();
    m_behaviorProcessor.Finalize();
    m_cameraProcessor.Finalize();
    m_renderProcessor.Finalize();

    // Scene管理の終了処理
    m_sceneManager.Finalize();

    // RenderViewの解放
    m_renderViews.clear();
}

// GameWorldの更新処理
void GameWorld::Update()
{
    // Scene管理の更新
    m_sceneManager.Update();
    IScene* scene = m_sceneManager.GetCurrentScene();

    // Processor群の更新
    m_physicsProcessor.Process(scene);  // 物理演算制御プロセッサー処理
    m_animationProcessor.Process(scene); // アニメーション制御プロセッサー処理
    m_behaviorProcessor.Process(scene); // Behavior制御プロセッサー処理
}

// GameWorldの描画処理
void GameWorld::Render()
{
    IScene* scene = m_sceneManager.GetCurrentScene();

    // デバッグ描画の収集
    m_physicsProcessor.CollectDebugDraw(scene);

    // カメラ設定・描画情報の取得
    m_cameraProcessor.Process(scene);
    m_cameraProcessor.GetRenderViews(m_renderViews);
    
    // 描画制御プロセッサー処理
    int numViews = m_cameraProcessor.GetCameraCounter();
    for (int i = 0; i < m_renderViews.size(); i++) {
        if (i >= numViews) break;
        RenderView& view = m_renderViews[i];
        m_renderProcessor.BindRenderView(&view);
        m_renderProcessor.Process(scene);
    }
}
