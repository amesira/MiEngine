//----------------------------------------------------
// manager.h [マネージャー]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//----------------------------------------------------
#ifndef MANAGER_H
#define MANAGER_H

// directx
#include <d3d11.h>
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

enum SCENE {
    SCENE_NONE = 0,
    SCENE_TITLE,
    SCENE_GAME,
    SCENE_RESULT,
    SCENE_MAX,
};

// メイン処理
void Manager_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Manager_Finalize();
void Manager_Update();
void Manager_Draw();

void Manager_SetGameLevel(int level);
void Manager_SendScore(int score);

void SetScene(SCENE scene);

#endif