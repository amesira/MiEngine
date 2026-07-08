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

#include "Engine/Asset/particle_system_asset.h"

class ParticleSystemComponent : public Component {
public:
    using SimulationSpace = ParticleSystemData::SimulationSpace;
    using ShapeType = ParticleSystemData::ShapeType;
    using BillboardMode = ParticleSystemData::BillboardMode;
    using BlendMode = ParticleSystemData::BlendMode;
    using MinMaxFloat = ParticleSystemData::MinMaxFloat;
    using MinMaxColor = ParticleSystemData::MinMaxColor;
    using FloatCurve = MiCurve::FloatCurve;
    using MainModule = ParticleSystemData::MainModule;
    using EmissionModule = ParticleSystemData::EmissionModule;
    using ShapeModule = ParticleSystemData::ShapeModule;
    using SizeOverLifetimeModule = ParticleSystemData::SizeOverLifetimeModule;
    using RendererModule = ParticleSystemData::RendererModule;

    // === パーティクルデータ定義 ===
    struct ParticleData {
        bool alive = false;         // パーティクルが生存しているかどうか
        float elapsedTime = 0.0f;   // パーティクルの経過時間
        float lifetime = 1.0f;      // パーティクルの寿命

        float startSize = 1.0f;     // パーティクルの初期サイズ
        float size = 1.0f;          // パーティクルのサイズ

        int frame = 0;                  // パーティクルのテクスチャアニメーションのフレーム

        XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };   // パーティクルの位置
        XMFLOAT3 velocity = { 0.0f, 0.0f, 0.0f };   // パーティクルの速度
        XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };// パーティクルの色
        XMFLOAT4 uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };        // パーティクルのUV矩形
        // RendererのUvRectと掛け合わせる
    };

    static constexpr int MAX_PARTICLES = 1000; // 最大パーティクル数

private:
    ParticleSystemAsset* m_asset = nullptr; // パーティクルシステムのアセット
    
    ParticleSystemDesc m_desc = {}; // パーティクルシステムの設定
    TextureResource* m_textureResource = nullptr; // パーティクルに使用するテクスチャ

    // パーティクルデータの配列
    std::vector<ParticleData> m_particles;

    // 再生状態
    bool m_isPlaying = false;

    float m_time = 0.0f;            // シミュレーション時間
    float m_emitAccumulator = 0.0f; // 端数の発生数を蓄積するアキュムレータ

    XMFLOAT3 m_previousPosition = { 0.0f, 0.0f, 0.0f }; // 前フレームの位置
    XMFLOAT3 m_currentPosition = { 0.0f, 0.0f, 0.0f };  // 現在の位置
    float m_distanceAccumulator = 0.0f;        // 距離あたりの発生のための距離アキュムレータ

public:
    ParticleSystemDesc& GetDesc() { return m_desc; }
    const ParticleSystemDesc& GetDesc() const { return m_desc; }

    TextureResource* GetTextureResource() { return m_textureResource ? m_textureResource : m_desc.rendererModule.textureResource; }
    const TextureResource* GetTextureResource() const { return m_textureResource ? m_textureResource : m_desc.rendererModule.textureResource; }
    void SetTextureResource(TextureResource* textureResource) {
        m_textureResource = textureResource;
        m_desc.rendererModule.textureResource = textureResource;
    }

    // メインモジュールへのアクセス
    MainModule& Main() { return m_desc.mainModule; }
    const MainModule& Main() const { return m_desc.mainModule; }

    //// エミッションモジュールへのアクセス
    EmissionModule& Emission() { return m_desc.emissionModule; }
    const EmissionModule& Emission() const { return m_desc.emissionModule; }

    //// 発生形状モジュールへのアクセス
    ShapeModule& Shape() { return m_desc.shapeModule; }
    const ShapeModule& Shape() const { return m_desc.shapeModule; }

    //// 寿命に応じたサイズ変化モジュールへのアクセス
    SizeOverLifetimeModule& SizeOverLifetime() { return m_desc.sizeOverLifetimeModule; }
    const SizeOverLifetimeModule& SizeOverLifetime() const { return m_desc.sizeOverLifetimeModule; }

    //// 描画モジュールへのアクセス
    RendererModule& Renderer() { return m_desc.rendererModule; }
    const RendererModule& Renderer() const { return m_desc.rendererModule; }

    // パーティクルデータへのアクセス
    std::vector<ParticleData>& Particles() { return m_particles; }
    const std::vector<ParticleData>& Particles() const { return m_particles; }

    // シミュレーション時間の設定・取得
    void SetTime(float time) { m_time = time; }
    float GetTime() const { return m_time; }

    // 端数の発生数アキュムレータの設定・取得
    void SetEmitAccumulator(float value) { m_emitAccumulator = value; }
    float GetEmitAccumulator() const { return m_emitAccumulator; }

    // 距離アキュムレータの設定・取得
    void SetDistanceAccumulator(float value) { m_distanceAccumulator = value; }
    float GetDistanceAccumulator() const { return m_distanceAccumulator; }

    // 前フレームの位置の設定・取得
    void SetPreviousPosition(const XMFLOAT3& position) { m_previousPosition = position; }
    const XMFLOAT3& GetPreviousPosition() const { return m_previousPosition; }

    // 現在の位置の設定・取得
    void SetCurrentPosition(const XMFLOAT3& position) { m_currentPosition = position; }
    const XMFLOAT3& GetCurrentPosition() const { return m_currentPosition; }

    // 再生
    void Play() { m_isPlaying = true; }
    // 一時停止
    void Stop() { m_isPlaying = false; }

    // 再生状態の取得
    bool IsPlaying() const { return m_isPlaying; }
};

#endif // PARTICLE_SYSTEM_COMPONENT_H
