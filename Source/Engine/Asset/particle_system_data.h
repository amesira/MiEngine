//---------------------------------------------------
// particle_system_data.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/28
//---------------------------------------------------
#pragma once
#include <DirectXMath.h>
using namespace DirectX;
#include <string>

#include "Utility/mi_curve.h"

class TextureResource;

namespace ParticleSystemData
{
    // === タイプ定義 ===
    // シミュレーション空間
    enum class SimulationSpace {
        Local,
        World,
    };

    // 発生形状
    enum class ShapeType {
        Sphere,
        Cone,
    };

    // ビルボードの種類
    enum class BillboardMode {
        View,       // 常にカメラに向く
        Horizontal, // 水平面に対して垂直
    };

    // ブレンドモード
    enum class BlendMode {
        AlphaBlend,
        Additive,
    };

    // 時間の種類（テクスチャアニメーションなどで使用）
    enum class TimeMode {
        Lifetime,
        Speed,
    };

    // === データ定義 ===
    // ランダムな値を生成するための構造体
    struct MinMaxFloat {
        bool randomBetweenTwoConstants = false;
        float constant = 1.0f;
        float constantMin = 1.0f;
        float constantMax = 1.0f;
    };

    // ランダムな色を生成するための構造体
    struct MinMaxColor {
        bool randomBetweenTwoColors = false;
        XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT4 colorMin = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT4 colorMax = { 1.0f, 1.0f, 1.0f, 1.0f };
    };

    // === モジュール定義 ===
    // パーティクルシステムの基本的な設定を行うモジュール
    struct MainModule {
        float duration = 5.0f;      // パーティクルシステムの再生時間
        bool loop = true;           // ループ再生するかどうか
        bool playOnAwake = true;    // ゲーム開始時に自動で再生するかどうか

        MinMaxFloat startLifetime = { false, 5.0f, 5.0f, 5.0f };    // パーティクルの寿命
        MinMaxFloat startSpeed = { false, 5.0f, 5.0f, 5.0f };       // パーティクルの初速
        MinMaxFloat startSize = { false, 1.0f, 1.0f, 1.0f };        // パーティクルの初期サイズ
        MinMaxColor startColor = {};                                // パーティクルの初期色

        XMFLOAT3 gravity = { 0.0f, 0.0f, 0.0f };    // 重力の影響
        float simulationSpeed = 1.0f;               // シミュレーションの速度
        SimulationSpace simulationSpace = SimulationSpace::Local; // シミュレーション空間
    };

    // パーティクルの発生に関する設定を行うモジュール
    struct EmissionModule {
        bool enabled = true;            // エミッションを有効にするかどうか
        float rateOverTime = 10.0f;     // 1秒あたりの発生数
        float rateOverDistance = 0.0f;  // 距離あたりの発生数
    };

    // 発生形状全般の設定を行うモジュール
    struct ShapeModule {
        bool enabled = true;                // 発生形状を有効にするかどうか
        ShapeType type = ShapeType::Sphere; // 発生形状の種類

        struct SphereShape {    // 球形状の発生設定
            float radius = 1.0f;
            bool emitFromShell = false;
        } sphere = {};
        struct ConeShape {      // 円錐形状の発生設定
            float angle = XM_PIDIV4;
            float radius = 1.0f;
            float length = 5.0f;
            bool emitFromBase = true; // 未実装
        } cone = {};

        float randomDirectionAmount = 0.0f; // 発生方向のランダムさ
    };

    // 寿命に応じたサイズの変化を設定するモジュール
    struct SizeOverLifetimeModule {
        bool enabled = false;
        MiCurve::FloatCurve size = {};
    };

    // テクスチャシートアニメーションの設定を行うモジュール
    struct TextureSheetAnimation {
        bool enabled = false;

        int tileX = 1;        // テクスチャの横方向の分割数
        int tileY = 1;        // テクスチャの縦方向の分割数p
        int startFrame = 0;     // アニメーションの開始フレーム
        int frameCount = 1;     // アニメーションの総フレーム数

        TimeMode timeMode = TimeMode::Lifetime; // アニメーションの時間の種類
        float framePerSecond = 30.0f; // 1秒あたりのフレーム数 : TimeMode::Speedの場合に使用

        bool loop = true;   // アニメーションをループさせるかどうか
    };

    // パーティクルの描画に関する設定を行うモジュール
    struct RendererModule {
        std::string texturePath;         // パーティクルに使用するテクスチャ
        TextureResource* textureResource = nullptr;
        XMFLOAT4 uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };       // テクスチャのUV矩形

        BillboardMode billboardMode = BillboardMode::View;  // ビルボードの種類
        BlendMode blendMode = BlendMode::AlphaBlend;        // ブレンドモード

        bool sortByDistance = true; // カメラからの距離でソートするかどうか 未使用
    };
}
