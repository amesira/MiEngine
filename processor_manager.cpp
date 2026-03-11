// processor_manager.cpp
#include "processor_manager.h"

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "shader.h"

#include "scene_interface.h"
#include "mi_fps.h"

#include "debug_renderer.h"

#include "physics_processor.h"

#include "camera_processor.h"
#include "render_processor.h"

#include "behavior_processor.h"

#include "game_object.h"
#include "image_component.h"

static PhysicsProcessor* g_PhysicsProcessor = nullptr;
static CameraProcessor* g_CameraProcessor = nullptr;
static RenderProcessor* g_RenderProcessor = nullptr;
static BehaviorProcessor* g_BehaviorProcessor = nullptr;

void ProcessorM_Initialize()
{
    DebugRenderer_Initialize();

    // Processorインスタンス化
    g_PhysicsProcessor = new PhysicsProcessor();
    g_CameraProcessor = new CameraProcessor();
    g_RenderProcessor = new RenderProcessor();
    g_BehaviorProcessor = new BehaviorProcessor();

    g_PhysicsProcessor->Initialize();
    g_CameraProcessor->Initialize();
    g_RenderProcessor->Initialize();
    g_BehaviorProcessor->Initialize();

    // ★ 現行設計ではInstanceスロットの初期化が何故かされていないため、ここで初期化しておく
    ClearInstanceData();
}

void ProcessorM_Finalize()
{
    DebugRenderer_Finalize();
    
    // 終了処理
   
    {
        g_PhysicsProcessor->Finalize();

        delete g_PhysicsProcessor;
        g_PhysicsProcessor = nullptr;
    }
   
    {
        g_CameraProcessor->Finalize();

        delete g_CameraProcessor;
        g_CameraProcessor = nullptr;
    }
    {
        g_RenderProcessor->Finalize();
        delete g_RenderProcessor;
        g_RenderProcessor = nullptr;
    }
    g_BehaviorProcessor->Finalize();
    delete g_BehaviorProcessor;
    g_BehaviorProcessor = nullptr;
}

void ProcessorM_Update(IScene* pScene)
{
    // 物理演算制御プロセッサー処理
    g_PhysicsProcessor->Process(pScene);

    // BehaviorProcessor処理
    g_BehaviorProcessor->Process(pScene);
}

void ProcessorM_Draw(IScene* pScene)
{
    // カメラ設定
    g_CameraProcessor->Process(pScene);
    auto renderViews = g_CameraProcessor->GetRenderViews();

    Direct3D_Clear();

    for (const RenderView& view : renderViews) {
        g_RenderProcessor->BindRenderView(view);
        g_RenderProcessor->Process(pScene);
    }
   
    // デバッグ描画用バッファリセット
    DebugRenderer_ResetBuffer();

    Direct3D_Present();

    //for(int i = 0; i < g_CameraProcessor->GetCameraCount(); i++) {
    //    // バッファのクリアとシーン描画用RTVのセット
    //    XMFLOAT4 clearColor = g_CameraProcessor->GetClearColor(i);
    //    Direct3D_BeginScene(clearColor.x, clearColor.y, clearColor.z);
    //    g_CameraProcessor->BindMatrix(i);

    //    // ライト設定
    //    g_LightProcessor->Process(pScene);

    //    // 各3D描画プロセッサーの実行
    //    SetDepthState(DEPTHSTATE_ENABLE);
    //    g_Renderer3DModelProcessor->Process(pScene);
    //    g_Renderer3DCubeProcessor->Process(pScene);

    //    // UI用ライト設定
    //    g_LightProcessor->SetUiLight();

    //    // world配置
    //    // -ワールド配置のUIは、必ず前面に描画される-
    //    SetBlendState(BLENDSTATE_ALFA);
    //    // ワールド配置のUI？

    //    // スナップショット撮影
    //    g_CameraProcessor->SnapShotCamera(i);
    //}
    //
    //Direct3D_Clear();

    //SetBlendState(BLENDSTATE_NONE);
    //SetDepthState(DEPTHSTATE_DISABLE);

    //g_LightProcessor->SetUiLight();

    //// カメラからのスナップショットをフルスクリーンに描画
    //const float w = (float)Direct3D_GetBackBufferWidth();
    //const float h = (float)Direct3D_GetBackBufferHeight();
    //g_CameraProcessor->DrawSnapshot(0, w / 2.0f, h / 2.0f, w, h);
    //SetBlendState(BLENDSTATE_ALFA);

    //// UI配置設定

   

    
}
