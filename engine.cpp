//===================================================
// engine.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "engine.h"

#include "direct3d.h"
#include "shader.h"
#include "sprite.h"
#include "Audio.h"
#include "manager.h"
#include "mi_fps.h"
#include "model.h"

bool MiEngine::Initialize(HWND hWnd)
{
    m_isRunning = true;

    // m_pAssetManager = new AssetManager();
    // m_pRenderer = new Renderer();
    // m_pInputSystem = new InputSystem();
    // m_pAudioSystem = new AudioSystem();
    // m_pGameWorld = new GameWorld();
    // m_pDebugWorld = new DebugWorld();

    Direct3D_Initialize(hWnd);

    ID3D11Device* pDevice = Direct3D_GetDevice();
    ID3D11DeviceContext* pContext = Direct3D_GetDeviceContext();

    Shader_Initialize(pDevice, pContext);
    InitializeSprite();

    InitAudio();

    Model_Initialize();

    Manager_Initialize(pDevice, pContext);

    FPS_Initialize(hWnd);

    return true;
}

void MiEngine::Finalize()
{
    // delete m_pAssetManager;
    // delete m_pRenderer;
    // delete m_pInputSystem;
    // delete m_pAudioSystem;
    // delete m_pGameWorld;
    // delete m_pDebugWorld;

    Manager_Finalize();

    UninitAudio();

    Model_Finalize();

    FinalizeSprite();
    Shader_Finalize();

    Direct3D_Finalize();
}

bool MiEngine::RunOneFrame()
{
    m_isRunning = true;

    if (FPS_Update()) {

        // 更新処理
        Manager_Update();

        // 描画処理
        Manager_Draw();

        // 処理回数更新
        FPS_UpdateFrameCount();

        return true;
    }

    return false;
}

//--------------------------------- private
// MiEngineの更新処理
void MiEngine::Update()
{

}

// MiEngineの描画処理
void MiEngine::Render()
{

}
