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
#include "mi_fps.h"
#include "model.h"

#include "game_world.h"

// MiEngineの初期化処理
bool MiEngine::Initialize(HWND hWnd)
{
    m_isRunning = true;

    Direct3D_Initialize(hWnd);

    ID3D11Device* pDevice = Direct3D_GetDevice();
    ID3D11DeviceContext* pContext = Direct3D_GetDeviceContext();

    Shader_Initialize(pDevice, pContext);
    InitializeSprite();
    InitAudio();
    Model_Initialize();
    FPS_Initialize(hWnd);

    // GameWorldの初期化
    m_pGameWorld = new GameWorld();
    m_pGameWorld->Initialize();

    return true;
}

// MiEngineの終了処理
void MiEngine::Finalize()
{
    // GameWorldの終了処理
    m_pGameWorld->Finalize();
    delete m_pGameWorld;
    m_pGameWorld = nullptr;

    UninitAudio();
    Model_Finalize();
    FinalizeSprite();
    Shader_Finalize();
    Direct3D_Finalize();
}

// MiEngineの1フレーム分の処理
bool MiEngine::RunOneFrame()
{
    if (!m_isRunning) return false;

    if (FPS_Update()) {
        Update();
        Render();
        FPS_UpdateFrameCount(); // フレームカウントを更新
        return true;
    }

    return false;
}

//--------------------------------- private
// MiEngineの更新処理
void MiEngine::Update()
{
    m_pGameWorld->Update();
}

// MiEngineの描画処理
void MiEngine::Render()
{
    m_pGameWorld->Render();
}
