#pragma once

#include <xaudio2.h>

void InitAudio();   // サウンドの初期化
void UninitAudio(); // サウンドの終了処理

int LoadAudio(const char* FileName);    // サウンドデータ読み込み
void UnloadAudio(int Index);            // サウンドデータ解放
void PlayAudio(int Index, bool Loop = false);   // サウンドデータ再生

// 使い方
// static int g_BgmID = NULL;
//
// g_BgmID = LoadAudio("asset\\Audio\\title.wav);
// PlayAudio(g_BgmID, true);