//===================================================
// direct3d.h [Direct3Dの初期化関連]
// 
// Author：Miu Kitamura
// Date  ：2025/05/11
//===================================================
#ifndef DIRECT3D_H
#define DIRECT3D_H

#include <Windows.h>
#include <d3d11.h>
#include <mmsystem.h>

#include <DirectXMath.h>

#include "DirectXTex.h"
#if _DEBUG
 #pragma comment(lib,"DirectXTex_Debug.lib")
#else
 #pragma comment(lib,"DirectXTex_Release.lib")
#endif

/* セーフリリースマクロ */
#define SAFE_RELEASE(o) if(o){(o)->Release();o=NULL;}

bool Direct3D_Initialize(HWND hWnd);    // DirectXの初期化
void Direct3D_Finalize();               // DirectXの終了処理

void Direct3D_Clear();  // バックバッファのクリア
void Direct3D_Present();// バックバッファの表示

void Direct3D_ResetViewport(); // ビューポートをリセット

/* ゲッター */
ID3D11Device* Direct3D_GetDevice();
ID3D11DeviceContext* Direct3D_GetDeviceContext();

unsigned int Direct3D_GetBackBufferWidth();
unsigned int Direct3D_GetBackBufferHeight();

/* ブレンドステート関連 */
enum BLENDSTATE {
    BLENDSTATE_NONE = 0,    // ブレンドしない
    BLENDSTATE_ALFA,        // 普通のαブレンド
    BLENDSTATE_ADD,         // 加算合成
    BLENDSTATE_SUB,         // 減算合成

    BLENDSTATE_MAX,
};
enum DEPTHSTATE {
    DEPTHSTATE_ENABLE = 0,  // デプス有効
    DEPTHSTATE_DISABLE,     // デプス無効
    DEPTHSTATE_NOWRITE,     // デプス書き込み無効
    DEPTHSTATE_MAX,
};

// ブレンドステートの設定
void SetBlendState(BLENDSTATE blend);
// デプスステートの設定
void SetDepthState(DEPTHSTATE depth);

// スナップショット用シーンテクスチャSRVの作成
void Direct3D_CreateSnapshotSceneSRV(ID3D11ShaderResourceView** snapshotSrv, ID3D11Texture2D** fromTex);

// シーンクリア
void Direct3D_ClearSceneTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);
// シーンセット
void Direct3D_SetSceneTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);

// シーン用RTV・SRVの作成・解放
void Direct3D_CreateColorBuffer(ID3D11Texture2D** tex, ID3D11RenderTargetView** rtv, ID3D11ShaderResourceView** srv);

// シーン用デプステンシルの作成・解放
void Direct3D_CreateDepthBuffer(ID3D11Texture2D** tex, ID3D11DepthStencilView** dsv, ID3D11ShaderResourceView** srv);

#endif