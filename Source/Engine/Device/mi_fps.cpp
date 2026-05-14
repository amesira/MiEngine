//===================================================
// fps.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/11/20
//===================================================
#include <SDKDDKVer.h>      // 利用できる最も上位のWindowsプラットフォームが定義される
#define WIN32_LEAN_AND_MEAN // 32bitアプリには不要な情報を抑止してコンパイル時間を短縮

#include "mi_fps.h"

#include <windows.h>
#include <algorithm>
#include "direct3d.h"

static HWND hWnd; // ウィンドウハンドル（main.cppで作成されたウィンドウの識別子）

// フレームレート計測用
static DWORD g_dwExecLastTime;
static DWORD g_dwFPSLastTime;
static DWORD g_dwCurrentTime;
static DWORD g_dwFrameCount;

int  g_CountFPS;        // FPSカウンター
char g_DebugStr[2048];  // FPS表示文字列

static float g_TimeScale = 1.0f;
static float g_DeltaTime = 0.0f;

#define MAX_DELTA_TIME (0.1f)

void FPS_Initialize(HWND h)
{
    hWnd = h;
    
    // フレームレート計測初期化
    timeBeginPeriod(1); // タイマーの精度を設定
    g_dwExecLastTime = g_dwFPSLastTime = timeGetTime(); // 現在のタイマー値（1000で1秒を表す）
    g_dwCurrentTime = g_dwFrameCount = 0;
}

void FPS_Finalize()
{

}

bool FPS_Update()
{
    g_dwCurrentTime = timeGetTime();
    if ((g_dwCurrentTime - g_dwFPSLastTime) >= 1000) {
        g_CountFPS = g_dwFrameCount;

        g_dwFPSLastTime = g_dwCurrentTime; // 現在のタイマー値を保存
        g_dwFrameCount = 0;
    }
    if ((g_dwCurrentTime - g_dwExecLastTime) >= ((float)1000 / 60)) {
        g_DeltaTime = (g_dwCurrentTime - g_dwExecLastTime) / 1000.0f;
        // 1/60s 経過した
        g_dwExecLastTime = g_dwCurrentTime; // 現在のタイマー値を保存
        return true;
    }
    return false;
}

void FPS_UpdateFrameCount()
{
    g_dwFrameCount++;
}

void FPS_SetTimeScale(float timeScale)
{
    g_TimeScale = timeScale;
}

float FPS_GetDeltaTime()
{
    if(g_DeltaTime > MAX_DELTA_TIME){
        g_DeltaTime = MAX_DELTA_TIME;
    }
    return g_DeltaTime * g_TimeScale;
}

float FPS_GetFPS()
{
    return g_CountFPS;
}

float FPS_GetUnscaledDeltaTime()
{
    if (g_DeltaTime > MAX_DELTA_TIME) {
        g_DeltaTime = MAX_DELTA_TIME;
    }
    return g_DeltaTime;
}
