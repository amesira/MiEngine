//===================================================
// game_world.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "game_world.h"
#include "debug_renderer.h"
#include "sprite.h"
#include "shader.h"

// GameWorldの初期化
void GameWorld::Initialize()
{
    // Scene管理の初期化
    m_sceneManager.Initialize();

    // Processor群の初期化
    m_physicsProcessor.Initialize();
    m_behaviorProcessor.Initialize();
    m_cameraProcessor.Initialize();
    m_renderProcessor.Initialize();

    // RenderViewの初期化
    m_renderViews.resize(8);
}

// GameWorldの終了処理
void GameWorld::Finalize()
{
    // Processor群の終了処理
    m_physicsProcessor.Finalize();
    m_behaviorProcessor.Finalize();
    m_cameraProcessor.Finalize();
    m_renderProcessor.Finalize();

    // Scene管理の終了処理
    m_sceneManager.Finalize();
}

// GameWorldの更新処理
void GameWorld::Update()
{
    DebugRenderer_ResetBuffer();

    // Scene管理の更新
    m_sceneManager.Update();
    IScene* scene = m_sceneManager.GetCurrentScene();

    // Processor群の更新
    m_physicsProcessor.Process(scene);  // 物理演算制御プロセッサー処理
    m_behaviorProcessor.Process(scene); // Behavior制御プロセッサー処理
}

// GameWorldの描画処理
void GameWorld::Render()
{
    IScene* scene = m_sceneManager.GetCurrentScene();

    // カメラ設定・描画情報の取得
    m_cameraProcessor.Process(scene);
    m_cameraProcessor.GetRenderViews(m_renderViews);
    
    // 描画制御プロセッサー処理
    for (RenderView& view : m_renderViews) {
        m_renderProcessor.BindRenderView(&view);
        m_renderProcessor.Process(scene);
    }

    Direct3D_Clear();

    Shader_Begin();
    SetBlendState(BLENDSTATE_ALFA);
    SetTexture(m_renderViews[0].colorBufferSRV.Get());

    float SCREEN_WIDTH = static_cast<float>(Direct3D_GetBackBufferWidth());
    float SCREEN_HEIGHT = static_cast<float>(Direct3D_GetBackBufferHeight());
    DrawSpriteScreen({SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, {SCREEN_WIDTH, SCREEN_HEIGHT}, {1,1,1,1}, {0,0,1,1});
}
