//===================================================
// game_world.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "game_world.h"
#include "Utility/debug_renderer.h"
#include "Engine/engine_service_locator.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Settings/scene_settings.h"

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

    // RenderViewの無効化
    for (RenderView& view : m_renderViews) {
        view.enabled = false;
    }

    // カメラ設定・描画情報の取得
    m_cameraProcessor.Process(scene);
    m_cameraProcessor.SetRenderViews(m_renderViews);
    m_mainGameRenderViewIndex = 0;

    // シーンカメラの描画情報をRenderViewに反映
    SetSceneRenderView(scene, m_cameraProcessor.GetCameraCounter());
    
    // 描画制御プロセッサー処理
    for (int i = 0; i < m_renderViews.size(); i++) {
        if (!m_renderViews[i].enabled) continue;

        RenderView& view = m_renderViews[i];
        m_renderProcessor.BindRenderView(&view);
        m_renderProcessor.Process(scene);
    }
}

// -------------------------------- private

// シーンカメラの描画情報をRenderViewに反映
void GameWorld::SetSceneRenderView(IScene* scene, int sceneRenderViewIndex)
{
    // シーンカメラの描画情報をRenderViewに反映
    SceneSettings& sceneSettings = scene->GetSceneSettings();
    sceneSettings.UpdateCameraSettings();
    const SceneCameraSettings& sceneCameraSettings = sceneSettings.GetCameraSettings();

    // シーン全体のレンダリング設定をRenderViewに反映
    int sceneViewIndex = sceneRenderViewIndex;
    if (sceneViewIndex >= m_renderViews.size()) {
        return;
    }

    RenderView& view = m_renderViews[sceneViewIndex];
    m_mainSceneRenderViewIndex = sceneViewIndex;
    view.enabled = true;

    view.viewMatrix = sceneCameraSettings.GetViewMatrix();
    view.projectionMatrix = sceneCameraSettings.GetProjectionMatrix();
    view.eyePosition = sceneCameraSettings.GetPosition();
    view.aspectRatio = sceneCameraSettings.GetAspect();

    view.enable3D = true;
    view.enableLighting = true;
    view.enablePostEffect = true;
    view.enableUI = false;
    view.enableDebugDraw = true;
    sceneViewIndex++;
    if (sceneViewIndex >= m_renderViews.size()) {
        return;
    }

    // Canvas用のRenderViewを有効化
    RenderView& canvasView = m_renderViews[sceneViewIndex];
    m_canvasRenderViewIndex = sceneViewIndex;
    canvasView.enabled = true;

    canvasView.enable3D = false;
    canvasView.enableLighting = false;
    canvasView.enablePostEffect = false;
    canvasView.enableUI = true;
}
