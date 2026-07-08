//---------------------------------------------------
// particle_system_asset.h
// ・ParticleSystemのアセットを表すクラスの定義
// 
// Author：Miu Kitamura
// Date  ：2026/06/28
//---------------------------------------------------
#pragma once
#include <string>
#include "particle_system_data.h"

 // 各モジュールのデータ
struct ParticleSystemDesc {
    ParticleSystemData::MainModule mainModule;
    ParticleSystemData::EmissionModule emissionModule;
    ParticleSystemData::ShapeModule shapeModule;
    ParticleSystemData::SizeOverLifetimeModule sizeOverLifetimeModule;
    ParticleSystemData::TextureSheetAnimation textureSheetAnimation;
    ParticleSystemData::RendererModule rendererModule;
};

class ParticleSystemAsset {
public:
    std::string name;
    int formatVersion = 1;  // 読み込み形式のver

private:
    ParticleSystemDesc m_desc = {};

public:
    const ParticleSystemDesc& GetDesc() const { return m_desc; }
    ParticleSystemDesc CopyDesc() const { return m_desc; }
};