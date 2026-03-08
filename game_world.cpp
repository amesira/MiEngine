//===================================================
// game_world.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "game_world.h"

//#include "scene_manager.h"
//
//#include "physics_processor.h"
//#include "camera_processor.h"
//#include "light_processor.h"
//#include "render_processor.h"

#include "processor_manager.h"

template<typename T>
void ReleaseProcessor(T*& p){
    static_assert(std::is_base_of<Processor, T>::value);

    if (p) {
        p->Finalize();
        delete p;
        p = nullptr;
    }
}

// GameWorldの初期化
void GameWorld::Initialize()
{
    // Scene管理の初期化
    m_pSceneManager = new SceneManager();
    m_pSceneManager->Initialize();

    //// Processor群の初期化
    //m_pPhysicsProcessor = new PhysicsProcessor();
    //m_pCameraProcessor = new CameraProcessor();
    //m_pLightProcessor = new LightProcessor();
    //m_pRenderProcessor = new RenderProcessor();

    ProcessorM_Initialize();
}

// GameWorldの終了処理
void GameWorld::Finalize()
{
    // Processor群の終了処理
   /* ReleaseProcessor(m_pPhysicsProcessor);
    ReleaseProcessor(m_pCameraProcessor);
    ReleaseProcessor(m_pLightProcessor);
    ReleaseProcessor(m_pRenderProcessor);*/

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
    Scene* scene = m_pSceneManager->GetCurrentScene();

    // Processor群の更新
    /*m_pPhysicsProcessor->Process(scene);
    m_pCameraProcessor->Process(scene);
    m_pLightProcessor->Process(scene);*/

    ProcessorM_Update(scene);
}

// GameWorldの描画処理
void GameWorld::Render()
{
    Scene* scene = m_pSceneManager->GetCurrentScene();

    // Processor群の描画処理
    //m_pRenderProcessor->Process(scene);

    ProcessorM_Draw(scene);
}