//----------------------------------------------------
// fps.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/20
//----------------------------------------------------
#ifndef MI_FPS_H
#define MI_FPS_H
#include <windows.h>

// FPS初期化・終了処理
void FPS_Initialize(HWND h);
void FPS_Finalize();
// FPS更新処理
bool FPS_Update();
void FPS_UpdateFrameCount();

// TimeScaleを設定
// ・0.0fなどにすることで、ポーズ画面などを楽に実装できる
void FPS_SetTimeScale(float timeScale);

float FPS_GetDeltaTime();
float FPS_GetFPS();

// TimeScaleの影響を受けないデルタタイムを取得
float FPS_GetUnscaledDeltaTime();

#endif