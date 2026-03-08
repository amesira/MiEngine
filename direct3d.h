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

void Direct3D_BeginScene(float r, float g, float b);

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
void Direct3D_CreateSnapshotSceneSRV(ID3D11ShaderResourceView** ppSrv);

// ビュー行列・プロジェクション行列の取得・設定
DirectX::XMMATRIX& Direct3D_GetViewMatrix();
DirectX::XMMATRIX& Direct3D_GetProjectionMatrix();
void Direct3D_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void Direct3D_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);

// カメラ位置の設定
void Direct3D_SetCameraInfo(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& atPosition);
DirectX::XMFLOAT3 Direct3D_GetCameraForward();
DirectX::XMFLOAT3 Direct3D_GetCameraRight();

#endif