//===================================================
// game_world.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "game_world.h"

#include "scene_manager.h"
#include "processor_manager.h"

// GameWorldの初期化
void GameWorld::Initialize()
{
    // Scene管理の初期化
    m_pSceneManager = new SceneManager();
    m_pSceneManager->Initialize();

    //// Processor群の初期化
    ProcessorM_Initialize();
}

// GameWorldの終了処理
void GameWorld::Finalize()
{
    // Processor群の終了処理
    ProcessorM_Finalize();

    // Scene管理の終了処理
    m_pSceneManager->Finalize();
    delete m_pSceneManager;
    m_pSceneManager = nullptr;
}

// GameWorldの更新処理
void GameWorld::Update()
{
    // Scene管理の更新
    m_pSceneManager->Update();
    IScene* scene = m_pSceneManager->GetCurrentScene();

    // Processor群の更新
    ProcessorM_Update(scene);
}

// GameWorldの描画処理
void GameWorld::Render()
{
    IScene* scene = m_pSceneManager->GetCurrentScene();

    // Processor群の描画処理
    ProcessorM_Draw(scene);
}

// GameWorldの観測
// void GameWorld::Observe(Camera camera)