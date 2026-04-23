//---------------------------------------------------
// debug_renderer.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/04
//---------------------------------------------------
#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

void DebugRenderer_Initialize();
void DebugRenderer_Finalize();

// 描画コマンドをバッファに追加
void DebugRenderer_DrawFlush(const XMMATRIX& view, const XMMATRIX& projection);

void DebugRenderer_ResetBuffer();

// ライン描画関数
// ・ここで描画情報をバッファに追加しておき、フレームの最後にまとめて描画するイメージ
void DebugRenderer_DrawLine(XMFLOAT3 start, XMFLOAT3 end, XMFLOAT4 color);

#endif // DEBUG_RENDERER_H