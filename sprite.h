//===================================================
// sprite.h [スプライト]
// 
// Author：Miu Kitamura
// Date  ：2025/06/17
//===================================================
#pragma once

// directX
#include <d3d11.h>
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

//----------------------------------------------------
// スプライト 初期化・終了処理
//----------------------------------------------------
/// @brief スプライトを初期化する
void InitializeSprite();

void LoadTexture(ID3D11ShaderResourceView** texture, const wchar_t* fileName);

/// @brief スプライトを終了・頂点バッファの解放
void FinalizeSprite();

/// @brief スプライト描画
void DrawSprite(XMFLOAT4 color,XMFLOAT4 uvRect,XMFLOAT3 normal = XMFLOAT3(0.0f,0.0f,1.0f));
/// @brief スプライト描画
/// @param position 位置
/// @param size 大きさ
/// @param color 色
/// @param uvRect UV座標
void DrawSpriteScreen(XMFLOAT2 position, XMFLOAT2 size, XMFLOAT4 color, XMFLOAT4 uvRect);

/// @brief テクスチャセット
void SetTexture(ID3D11ShaderResourceView* texture);

/// @brief インスタンシング描画準備
void    PrepareDrawInstance();
/// @brief インスタンシングデータ追加
/// @param world ワールド座標
/// @param color 色
/// @param uvRect UV座標
void    AddInstanceData(const XMMATRIX& world, const XMFLOAT4& color, const XMFLOAT4& uvRect);
/// @brief インスタンシング描画命令
void    DrawInstance();

/// @brief インスタンシングデータクリア
void    ClearInstanceData();

void    SetWorldMatrix(const XMMATRIX& world);