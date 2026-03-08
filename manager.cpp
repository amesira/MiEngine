//===================================================
// manager.cpp [マネージャー]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "manager.h"

#include "game.h"

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static IScene* g_SceneInstance[SCENE::SCENE_MAX];
static SCENE g_Scene = SCENE::SCENE_NONE;

//===================================================
// スコア初期化処理
//===================================================
void Manager_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (!g_pDevice)g_pDevice = pDevice;
    if (!g_pContext)g_pContext = pContext;

    g_SceneInstance[SCENE::SCENE_NONE] = nullptr;
    g_SceneInstance[SCENE::SCENE_GAME] = new GameScene();

    SetScene(SCENE::SCENE_GAME);
}

//===================================================
// スコア終了処理
//===================================================
void Manager_Finalize()
{
    SetScene(SCENE::SCENE_NONE);
}

//===================================================
// スコア更新処理
//===================================================
void Manager_Update()
{
    g_SceneInstance[g_Scene]->Update();
}

//===================================================
// スコア描画処理
//===================================================
void Manager_Draw()
{
    g_SceneInstance[g_Scene]->Draw();
}

void Manager_SetGameLevel(int level)
{
   
}

void Manager_SendScore(int score)
{
   
}

void SetScene(SCENE scene)
{
    // 現在のシーンの終了処理
    if (g_Scene != SCENE::SCENE_NONE){
        g_SceneInstance[g_Scene]->Finalize();
    }

    // シーン切り替え
    g_Scene = scene;

    // 次のシーンの初期化
    if (g_SceneInstance[g_Scene] == nullptr)return;
    g_SceneInstance[g_Scene]->Initialize();
}