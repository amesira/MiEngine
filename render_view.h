// render_view.h
#pragma once
#include <direct.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

// 描画に必要な情報をまとめる構造体
struct RenderView {
    // カメラ情報
    XMMATRIX viewMatrix;        // ビュー行列
    XMMATRIX projectionMatrix;  // プロジェクション行列

    // 描画処理の有効・無効
    bool enableLighting;      // ライトの有効・無効
    bool enableUI;            // UI描画の有効・無効
};
