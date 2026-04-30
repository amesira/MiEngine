//===================================================
// engine.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "engine.h"

#include "Engine/System/Device/direct3d.h"
#include "Engine/System/Device/Audio.h"
#include "Engine/System/Device/mi_fps.h"

#include "Engine/Core/scene_interface.h"

#include "Utility/debug_renderer.h"

#include "Engine/engine_service_locator.h"

// MiEngineの初期化処理
bool MiEngine::Initialize(HWND hWnd)
{
    m_isRunning = true;

    // サービスロケーターにエンジンインスタンスをセット
    EngineServiceLocator::s_engineInstance = this;

    // Direct3Dの初期化
    Direct3D_Initialize(hWnd);
    ID3D11Device* pDevice = Direct3D_GetDevice();
    ID3D11DeviceContext* pContext = Direct3D_GetDeviceContext();

    // マネージャー・システムの初期化
    m_shaderManager.Initialize(pDevice, pContext);
    InitAudio();
    FPS_Initialize(hWnd);
    m_resourceManager.Initialize();
    DebugRenderer_Initialize();

    // GameWorldの初期化
    m_gameWorld.Initialize();

    // Editorの初期化
    m_editorManager.Initialize(hWnd);
    m_editorContext = &(m_editorManager.GetEditorContext());

    return true;
}

// MiEngineの終了処理
void MiEngine::Finalize()
{
    // GameWorldの終了処理
    m_gameWorld.Finalize();

    m_resourceManager.Finalize();
    m_shaderManager.Finalize();
    UninitAudio();
    Direct3D_Finalize();
    DebugRenderer_Finalize();

    // サービスロケーターのエンジンインスタンスをクリア
    EngineServiceLocator::s_engineInstance = nullptr;
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
    // シーンのリロード要求がある場合はシーンをリロード
    // （SceneManager内の切り替え処理とは別に、エディタから強制的にシーンをリロードしたい場合に使用）
    if (m_editorContext->triggerSceneReload) {
        m_editorContext->triggerSceneReload = false;
        m_gameWorld.GetSceneManager().ReloadScene();
        m_editorContext->selectedObject = nullptr; // 選択オブジェクトをリセット
    }

    // デバッグ描画のバッファリセット
    DebugRenderer_ResetBuffer();

    // PlayモードのときのみGameWorldを更新
    if (m_editorContext->currentEditorMode == EditorContext::EditorMode::Play) {
        m_gameWorld.Update();
    }
}

// MiEngineの描画処理
void MiEngine::Render()
{
    // GameWorldの描画処理
    m_gameWorld.Render();

    Direct3D_Clear();

    // Editorの描画処理
    IScene* scene = m_gameWorld.GetSceneManager().GetCurrentScene();
    m_editorContext->scene = scene;
    m_editorContext->sceneSettings = &(scene->GetSceneSettings());
    m_editorContext->sceneRenderView = &m_gameWorld.GetMainSceneRenderView();
    m_editorContext->gameRenderView = &m_gameWorld.GetMainGameRenderView();
    m_editorContext->canvasRenderView = &m_gameWorld.GetCanvasRenderView();
    m_editorManager.Render();

    Direct3D_Present();
}
