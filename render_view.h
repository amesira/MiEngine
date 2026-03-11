// render_view.h
#pragma once
#include <direct.h>
#include <d3d11.h>
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

#include "Windows.h"
#include "wrl/client.h"
using Microsoft::WRL::ComPtr;

// 描画に必要な情報をまとめる構造体
struct RenderView {
    // カメラ情報
    XMMATRIX viewMatrix;        // ビュー行列
    XMMATRIX projectionMatrix;  // プロジェクション行列

    // 描画処理の有効・無効
    bool enableLighting;      // ライトの有効・無効
    bool enableShadowMap;     // シャドウマップの有効・無効
    bool enablePostEffect;    // ポストエフェクトの有効・無効
    bool enableUI;            // UI描画の有効・無効

    // カラーバッファ
    ComPtr<ID3D11Texture2D>             colorBufferTexture;
    ComPtr<ID3D11RenderTargetView>      colorBufferRTV;
    ComPtr<ID3D11ShaderResourceView>    colorBufferSRV;
    // デプスバッファ
    ComPtr<ID3D11Texture2D>             depthBufferTexture;
    ComPtr<ID3D11DepthStencilView>      depthBufferDSV;

    // ポストエフェクト用一時バッファ
    ComPtr<ID3D11Texture2D>             postEffectTexture;
    ComPtr<ID3D11RenderTargetView>      postEffectRTV;
    ComPtr<ID3D11ShaderResourceView>    postEffectSRV;

    // スナップショット用のフラグ・SRV
    // bool afterPostEffectSnapshotRequested;
    // ・どのタイミングでスナップショットを取るか？のトリガーなどを作っておくと良いのかも
    // ・enumでも良い気もするが

    RenderView() :
        viewMatrix(XMMatrixIdentity()),
        projectionMatrix(XMMatrixIdentity()),
        enableLighting(true),
        enableShadowMap(true),
        enablePostEffect(true),
        enableUI(true) {
        Direct3D_CreateSceneTexture(&colorBufferTexture, &colorBufferSRV, &colorBufferRTV);
        Direct3D_CreateSceneTexture(&postEffectTexture, &postEffectSRV, &postEffectRTV);
    }
};