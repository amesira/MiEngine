//---------------------------------------------------
// particle_system_component.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//---------------------------------------------------
#ifndef PARTICLE_SYSTEM_COMPONENT_H
#define PARTICLE_SYSTEM_COMPONENT_H

#include "Engine/Core/component.h"
#include "Engine/Device/direct3d.h"
#include "Engine/Graphics/texture_resource.h"

#include <vector>

using namespace DirectX;

class ParticleSystemComponent : public Component {
public:
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

    // カーブのキー
    struct CurveKey {
        float time = 0.0f;
        float value = 1.0f;
    };
    // カーブ
    struct FloatCurve {
        std::vector<CurveKey> keys = {
            { 0.0f, 1.0f },
            { 1.0f, 1.0f },
        };
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
            bool emitFromBase = true;
        } cone = {};

        float randomDirectionAmount = 0.0f; // 発生方向のランダムさ
    };

    // 寿命に応じたサイズの変化を設定するモジュール
    struct SizeOverLifetimeModule {
        bool enabled = false;
        FloatCurve size = {};
    };

    // パーティクルの描画に関する設定を行うモジュール
    struct RendererModule {
        TextureResource* textureResource = nullptr;         // パーティクルに使用するテクスチャ
        XMFLOAT4 uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };       // テクスチャのUV矩形

        BillboardMode billboardMode = BillboardMode::View;  // ビルボードの種類
        BlendMode blendMode = BlendMode::AlphaBlend;        // ブレンドモード

        bool sortByDistance = true; // カメラからの距離でソートするかどうか
    };

    // === パーティクルデータ定義 ===
    struct ParticleData {
        bool alive = false;         // パーティクルが生存しているかどうか
        float elapsedTime = 0.0f;   // パーティクルの経過時間
        float lifetime = 1.0f;      // パーティクルの寿命

        float startSize = 1.0f;     // パーティクルの初期サイズ
        float size = 1.0f;          // パーティクルのサイズ

        XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };   // パーティクルの位置
        XMFLOAT3 velocity = { 0.0f, 0.0f, 0.0f };   // パーティクルの速度
        XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };// パーティクルの色
    };

    static constexpr int MAX_PARTICLES = 1000; // 最大パーティクル数

private:
    // メインモジュール
    MainModule m_main = {};
    // エミッションモジュール
    EmissionModule m_emission = {};
    // 発生形状モジュール
    ShapeModule m_shape = {};
    // 寿命に応じたサイズ変化モジュール
    SizeOverLifetimeModule m_sizeOverLifetime = {};
    // 描画モジュール
    RendererModule m_renderer = {};

    // パーティクルデータの配列
    std::vector<ParticleData> m_particles;
    // シミュレーションの時間管理
    float m_time = 0.0f;
    // 端数の発生数を管理するためのアキュムレータ
    float m_emitAccumulator = 0.0f;

    // 再生状態
    bool m_isPlaying = false;

public:
    // メインモジュールへのアクセス
    MainModule& Main() { return m_main; }
    const MainModule& Main() const { return m_main; }

    // エミッションモジュールへのアクセス
    EmissionModule& Emission() { return m_emission; }
    const EmissionModule& Emission() const { return m_emission; }

    // 発生形状モジュールへのアクセス
    ShapeModule& Shape() { return m_shape; }
    const ShapeModule& Shape() const { return m_shape; }

    // 寿命に応じたサイズ変化モジュールへのアクセス
    SizeOverLifetimeModule& SizeOverLifetime() { return m_sizeOverLifetime; }
    const SizeOverLifetimeModule& SizeOverLifetime() const { return m_sizeOverLifetime; }

    // 描画モジュールへのアクセス
    RendererModule& Renderer() { return m_renderer; }
    const RendererModule& Renderer() const { return m_renderer; }

    // パーティクルデータへのアクセス
    std::vector<ParticleData>& Particles() { return m_particles; }
    const std::vector<ParticleData>& Particles() const { return m_particles; }

    // シミュレーション時間の設定・取得
    void SetTime(float time) { m_time = time; }
    float GetTime() const { return m_time; }

    // 端数の発生数アキュムレータの設定・取得
    void SetEmitAccumulator(float value) { m_emitAccumulator = value; }
    float GetEmitAccumulator() const { return m_emitAccumulator; }

    // 再生
    void Play() { m_isPlaying = true; }
    // 一時停止
    void Stop() { m_isPlaying = false; }

    // 再生状態の取得
    bool IsPlaying() const { return m_isPlaying; }
};

#endif // PARTICLE_SYSTEM_COMPONENT_H
