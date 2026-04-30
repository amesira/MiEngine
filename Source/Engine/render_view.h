// render_view.h
#pragma once
#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

#include "Windows.h"
#include "wrl/client.h"
using Microsoft::WRL::ComPtr;

// 描画に必要な情報をまとめる構造体
struct RenderView {
    // このRenderViewが有効かどうか
    bool enabled = false;

    // カメラ情報
    XMMATRIX viewMatrix;        // ビュー行列
    XMMATRIX projectionMatrix;  // プロジェクション行列
    XMFLOAT3 eyePosition;       // カメラの位置
    float   aspectRatio;        // アスペクト比

    // 描画処理の有効・無効
    bool enable3D;            // 3D描画の有効・無効
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
    ComPtr<ID3D11ShaderResourceView>    depthBufferSRV;

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
        eyePosition(0, 0, 0),
        aspectRatio(16.0f / 9.0f),
        enable3D(true),
        enableLighting(true),
        enableShadowMap(true),
        enablePostEffect(true),
        enableUI(true) {
    }

    void Initialize() {
        Direct3D_CreateColorBuffer(colorBufferTexture.GetAddressOf(), colorBufferRTV.GetAddressOf(), colorBufferSRV.GetAddressOf());
        Direct3D_CreateDepthBuffer(depthBufferTexture.GetAddressOf(), depthBufferDSV.GetAddressOf(), depthBufferSRV.GetAddressOf());
        Direct3D_CreateColorBuffer(postEffectTexture.GetAddressOf(), postEffectRTV.GetAddressOf(), postEffectSRV.GetAddressOf());
    }
};