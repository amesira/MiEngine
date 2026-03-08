//---------------------------------------------------
// debug_renderer.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/04
//---------------------------------------------------
#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

void DebugRenderer_Initialize();
void DebugRenderer_Finalize();

// 描画コマンドをバッファに追加
void DebugRenderer_DrawFlush();

void DebugRenderer_ResetBuffer();

// ライン描画関数
// ・ここで描画情報をバッファに追加しておき、フレームの最後にまとめて描画するイメージ
void DrawLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color);

#endif // DEBUG_RENDERER_H