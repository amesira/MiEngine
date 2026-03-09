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

#include "renderer_3dcube_processor.h"
#include "renderer_3dmodel_processor.h"
#include "physics_processor.h"
#include "collision_processor.h"
#include "dynamics_processor.h"
#include "renderer_font_processor.h"
#include "renderer_image_processor.h"
#include "camera_processor.h"
#include "light_processor.h"
#include "renderer_slider_processor.h"

#include "behavior_processor.h"

#include "game_object.h"
#include "image_component.h"

static Renderer3DCubeProcessor* g_Renderer3DCubeProcessor = nullptr;
static Renderer3DModelProcessor* g_Renderer3DModelProcessor = nullptr;

static PhysicsProcessor* g_PhysicsProcessor = nullptr;
static CollisionProcessor* g_CollisionProcessor = nullptr;
static DynamicsProcessor* g_DynamicsProcessor = nullptr;

static RendererFontProcessor* g_RendererFontProcessor = nullptr;
static RendererImageProcessor* g_RendererImageProcessor = nullptr;
static RendererSliderProcessor* g_RendererSliderProcessor = nullptr;

static CameraProcessor* g_CameraProcessor = nullptr;
static LightProcessor* g_LightProcessor = nullptr;

static BehaviorProcessor* g_BehaviorProcessor = nullptr;

void ProcessorM_Initialize()
{
    DebugRenderer_Initialize();

    // Processorインスタンス化
    g_Renderer3DCubeProcessor = new Renderer3DCubeProcessor();
    g_Renderer3DModelProcessor = new Renderer3DModelProcessor();

    g_PhysicsProcessor = new PhysicsProcessor();
    g_CollisionProcessor = new CollisionProcessor();
    g_DynamicsProcessor = new DynamicsProcessor();

    g_RendererFontProcessor = new RendererFontProcessor();
    g_RendererImageProcessor = new RendererImageProcessor();
    g_RendererSliderProcessor = new RendererSliderProcessor();

    g_CameraProcessor = new CameraProcessor();
    g_LightProcessor = new LightProcessor();

    g_BehaviorProcessor = new BehaviorProcessor();

    // Processor初期化
    {   // 3D描画系プロセッサー初期化
        g_Renderer3DCubeProcessor->Initialize();
        g_Renderer3DModelProcessor->Initialize();
    }
    {   // 物理演算系プロセッサー初期化
        g_PhysicsProcessor->Initialize();
        g_CollisionProcessor->Initialize();
        g_DynamicsProcessor->Initialize();
    }
    {   // 2D描画系プロセッサー初期化
        g_RendererFontProcessor->Initialize();
        g_RendererImageProcessor->Initialize();
        g_RendererSliderProcessor->Initialize();
    }
    {   // カメラ・ライト系プロセッサー初期化
        g_CameraProcessor->Initialize();
        g_LightProcessor->Initialize();
    }
    g_BehaviorProcessor->Initialize();

    // ★ 現行設計ではInstanceスロットの初期化が何故かされていないため、ここで初期化しておく
    EndDrawInstance();
}

void ProcessorM_Finalize()
{
    DebugRenderer_Finalize();
    
    // 終了処理
    {
        g_Renderer3DCubeProcessor->Finalize();
        g_Renderer3DModelProcessor->Finalize();

        delete g_Renderer3DCubeProcessor;
        g_Renderer3DCubeProcessor = nullptr;
        delete g_Renderer3DModelProcessor;
        g_Renderer3DModelProcessor = nullptr;
    }
    {
        g_PhysicsProcessor->Finalize();
        g_CollisionProcessor->Finalize();
        g_DynamicsProcessor->Finalize();

        delete g_PhysicsProcessor;
        g_PhysicsProcessor = nullptr;
        delete g_CollisionProcessor;
        g_CollisionProcessor = nullptr;
        delete g_DynamicsProcessor;
        g_DynamicsProcessor = nullptr;
    }
    {
        g_RendererFontProcessor->Finalize();
        g_RendererImageProcessor->Finalize();
        g_RendererSliderProcessor->Finalize();

        delete g_RendererFontProcessor;
        g_RendererFontProcessor = nullptr;
        delete g_RendererImageProcessor;
        g_RendererImageProcessor = nullptr;
        delete g_RendererSliderProcessor;
        g_RendererSliderProcessor = nullptr;
    }
    {
        g_CameraProcessor->Finalize();
        g_LightProcessor->Finalize();

        delete g_CameraProcessor;
        g_CameraProcessor = nullptr;
        delete g_LightProcessor;
        g_LightProcessor = nullptr;
    }
    g_BehaviorProcessor->Finalize();
    delete g_BehaviorProcessor;
    g_BehaviorProcessor = nullptr;
}

void ProcessorM_Update(IScene* pScene)
{
    // 物理演算制御プロセッサー処理
    g_PhysicsProcessor->Process(pScene);
    g_CollisionProcessor->Process(pScene);
    g_DynamicsProcessor->Process(pScene);

    // BehaviorProcessor処理
    g_BehaviorProcessor->Process(pScene);
}

void ProcessorM_Draw(IScene* pScene)
{
    // カメラ設定
    g_CameraProcessor->Process(pScene);

    // ワールド配置設定
    g_RendererImageProcessor->SetDrawWorldImages(true);
    g_RendererImageProcessor->SetDrawUiImages(false);

    for(int i = 0; i < g_CameraProcessor->GetCameraCount(); i++) {
        // バッファのクリアとシーン描画用RTVのセット
        XMFLOAT4 clearColor = g_CameraProcessor->GetClearColor(i);
        Direct3D_BeginScene(clearColor.x, clearColor.y, clearColor.z);
        g_CameraProcessor->BindMatrix(i);

        // ライト設定
        g_LightProcessor->Process(pScene);

        // 各3D描画プロセッサーの実行
        SetDepthState(DEPTHSTATE_ENABLE);
        g_Renderer3DModelProcessor->Process(pScene);
        g_Renderer3DCubeProcessor->Process(pScene);

        // UI用ライト設定
        g_LightProcessor->SetUiLight();

        // world配置
        // -ワールド配置のUIは、必ず前面に描画される-
        SetBlendState(BLENDSTATE_ALFA);
        g_RendererImageProcessor->Process(pScene);

        // スナップショット撮影
        g_CameraProcessor->SnapShotCamera(i);
    }
    
    Direct3D_Clear();

    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    g_LightProcessor->SetUiLight();

    // カメラからのスナップショットをフルスクリーンに描画
    const float w = (float)Direct3D_GetBackBufferWidth();
    const float h = (float)Direct3D_GetBackBufferHeight();
    g_CameraProcessor->DrawSnapshot(0, w / 2.0f, h / 2.0f, w, h);
    SetBlendState(BLENDSTATE_ALFA);

    // UI配置設定
    g_RendererImageProcessor->SetDrawWorldImages(false);
    g_RendererImageProcessor->SetDrawUiImages(true);

    // 各2D描画プロセッサーの実行
    g_RendererImageProcessor->Process(pScene);
    g_RendererSliderProcessor->Process(pScene);
    g_RendererFontProcessor->Process(pScene);

    // デバッグ描画用バッファリセット
    DebugRenderer_ResetBuffer();

    Direct3D_Present();
}
