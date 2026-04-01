//===================================================
// engine.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "engine.h"

#include "direct3d.h"
#include "Audio.h"
#include "mi_fps.h"

#include "debug_renderer.h"

#include "scene_view_window.h"
#include "inspector_view_window.h"
#include "hierarchy_view_window.h"
#include "tool_bar_window.h"
#include "debug_view_window.h"

#include "engine_service_locator.h"

// MiEngineの初期化処理
bool MiEngine::Initialize(HWND hWnd)
{
    m_isRunning = true;

    // サービスロケーターにエンジンインスタンスをセット
    EngineServiceLocator::s_engineInstance = this;

    Direct3D_Initialize(hWnd);

    ID3D11Device* pDevice = Direct3D_GetDevice();
    ID3D11DeviceContext* pContext = Direct3D_GetDeviceContext();

   /* Shader_Initialize(pDevice, pContext);
    InitializeSprite();*/
    m_shaderManager.Initialize(pDevice, pContext);
    InitAudio();
    FPS_Initialize(hWnd);

    m_resourceManager.Initialize();

    DebugRenderer_Initialize();

    // GameWorldの初期化
    m_gameWorld.Initialize();

    // ImGuiの初期化
    m_imguiManager.Initialize(hWnd);
    {
        m_editorContext.displayX = 1920.0f;
        m_editorContext.displayY = 1080.0f;

        SceneViewWindow sceneViewWindow(&m_editorContext);
        m_imguiManager.AddWindow(std::make_unique<SceneViewWindow>(sceneViewWindow));

        InspectorViewWindow inspectorViewWindow(&m_editorContext);
        m_imguiManager.AddWindow(std::make_unique<InspectorViewWindow>(inspectorViewWindow));

        HierarchyViewWindow hierarchyViewWindow(&m_editorContext);
        m_imguiManager.AddWindow(std::make_unique<HierarchyViewWindow>(hierarchyViewWindow));

        DebugViewWindow debugViewWindow(&m_editorContext);
        m_imguiManager.AddWindow(std::make_unique<DebugViewWindow>(debugViewWindow));

        ToolBarWindow toolBarWindow(&m_editorContext);
        m_imguiManager.AddWindow(std::make_unique<ToolBarWindow>(toolBarWindow));
    }

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
    /*FinalizeSprite();
    Shader_Finalize();*/
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
    if (m_editorContext.triggerSceneReload) {
        m_editorContext.triggerSceneReload = false;
        m_gameWorld.GetSceneManager().ReloadScene();

        m_editorContext.selectedObject = nullptr; // 選択オブジェクトをリセット
    }

    if (m_editorContext.currentEditorMode == EditorContext::EditorMode::Play) {
        m_gameWorld.Update();
    }
}

// MiEngineの描画処理
void MiEngine::Render()
{
    m_gameWorld.Render();

    // ImGuiの描画
    m_editorContext.scene = m_gameWorld.GetSceneManager().GetCurrentScene();
    m_editorContext.sceneRenderView = &m_gameWorld.GetRenderViews()[0];
    m_imguiManager.RenderProcess();

    Direct3D_Present();
}
