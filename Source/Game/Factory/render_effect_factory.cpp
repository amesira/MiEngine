//===================================================
// render_effect_factory.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//===================================================
#include "render_effect_factory.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_base.h"
#include "Engine/Core/scene_interface.h"

// component
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/decal_component.h"
#include "Engine/Framework/Component/particle_system_component.h"

#include "Game/Behavior/transform_constraint_behavior.h"

#include "Engine/engine_service_locator.h"
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

// デカールエフェクト生成
GameObject* RenderEffectFactory::CreateDecalEffect(SceneBase* scene, const XMFLOAT3& position, const std::wstring& decalTexturePath)
{
    GameObject* decalEffect = scene->CreateGameObject();
    decalEffect->SetName("DecalEffect");

    // component生成・登録
    TransformComponent* transform = decalEffect->AddComponent<TransformComponent>();
    DecalComponent* decal = decalEffect->AddComponent<DecalComponent>();

    // component設定
    transform->SetPosition(position);
    TextureResource* decalTexture = TEXTURE_REPOSITORY->GetTextureResource(decalTexturePath);
    decal->SetDecalTexture(decalTexture);

    return decalEffect;
}

GameObject* RenderEffectFactory::CreateParticleEffect(SceneBase* scene, const XMFLOAT3& position, const std::wstring& texturePath)
{
    GameObject* particleEffect = scene->CreateGameObject();
    particleEffect->SetName("ParticleEffect");
    particleEffect->SetRenderLayer(RenderLayer::Particle);

    TransformComponent* transform = particleEffect->AddComponent<TransformComponent>();
    ParticleSystemComponent* particleSystem = particleEffect->AddComponent<ParticleSystemComponent>();

    transform->SetPosition(position);

    auto& desc = particleSystem->GetDesc();
    auto& main = desc.mainModule;
    main.duration = 3.0f;
    main.loop = true;
    main.playOnAwake = true;
    main.startLifetime = { true, 1.2f, 0.8f, 1.8f };
    main.startSpeed = { true, 1.5f, 0.8f, 2.4f };
    main.startSize = { true, 0.35f, 0.2f, 0.65f };
    main.startColor.randomBetweenTwoColors = true;
    main.startColor.colorMin = { 1.0f, 0.45f, 0.12f, 0.85f };
    main.startColor.colorMax = { 1.0f, 0.95f, 0.35f, 0.45f };
    main.gravity = { 0.0f, -0.25f, 0.0f };
    main.simulationSpeed = 1.0f;
    main.simulationSpace = ParticleSystemData::SimulationSpace::World;

    auto& emission = desc.emissionModule;
    emission.enabled = true;
    emission.rateOverTime = 24.0f;
    emission.rateOverDistance = 0.0f;

    auto& shape = desc.shapeModule;
    shape.enabled = true;
    shape.type = ParticleSystemData::ShapeType::Cone;
    shape.cone.angle = XM_PIDIV4;
    shape.cone.radius = 0.35f;
    shape.cone.emitFromBase = true;
    shape.randomDirectionAmount = 0.2f;

    auto& sizeOverLifetime = desc.sizeOverLifetimeModule;
    sizeOverLifetime.enabled = true;
    sizeOverLifetime.size.keys = {
        { 0.0f, 0.35f },
        { 0.25f, 1.0f },
        { 1.0f, 0.0f },
    };

    auto& renderer = desc.rendererModule;
    particleSystem->SetTextureResource(TEXTURE_REPOSITORY->GetTextureResource(texturePath));
    renderer.uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    renderer.billboardMode = ParticleSystemData::BillboardMode::View;
    renderer.blendMode = ParticleSystemData::BlendMode::Additive;
    renderer.sortByDistance = true;

    return particleEffect;
}

/// @brief 土ぼこりパーティクル生成
GameObject* RenderEffectFactory::CreateRunDustParticle(SceneBase* scene,std::string targetName)
{
    if (!scene) return nullptr;

    GameObject* target = scene->GetGameObjectByName(targetName);
    if (!target) return nullptr;
    TransformComponent* targetTransform = target->GetComponent<TransformComponent>();
    if (!targetTransform) return nullptr;

    GameObject* dustEffect = scene->CreateGameObject();
    dustEffect->SetName("RunDustParticle");
    dustEffect->SetRenderLayer(RenderLayer::Particle);

    TransformComponent* transform = dustEffect->AddComponent<TransformComponent>();
    ParticleSystemComponent* particleSystem = dustEffect->AddComponent<ParticleSystemComponent>();

    auto& desc = particleSystem->GetDesc();
    auto& main = desc.mainModule;
    main.duration = 1.0f;
    main.loop = true;
    main.playOnAwake = true;
    main.startLifetime = { true, 0.45f, 0.25f, 0.65f };
    main.startSpeed = { true, 0.45f, 0.2f, 0.8f };
    main.startSize = { true, 1.35f, 1.2f, 1.55f };
    main.startColor.randomBetweenTwoColors = true;
    main.startColor.colorMin = { 0.92f, 0.94f, 0.94f, 0.75f };
    main.startColor.colorMax = { 1.00f, 1.00f, 1.00f, 0.95f };
    main.gravity = { 0.0f, -0.25f, 0.0f };
    main.simulationSpeed = 1.0f;
    main.simulationSpace = ParticleSystemData::SimulationSpace::World;

    auto& emission = desc.emissionModule;
    emission.enabled = true;
    emission.rateOverTime = 0.0f;
    emission.rateOverDistance = 4.0f;

    auto& shape = desc.shapeModule;
    shape.enabled = true;
    shape.type = ParticleSystemData::ShapeType::Sphere;
    shape.sphere.radius = 0.18f;
    shape.sphere.emitFromShell = false;
    shape.randomDirectionAmount = 0.65f;

    auto& sizeOverLifetime = desc.sizeOverLifetimeModule;
    sizeOverLifetime.enabled = true;
    sizeOverLifetime.size.keys = {
        { 0.0f, 0.25f },
        { 0.35f, 1.0f },
        { 1.0f, 0.15f },
    };

    auto& texSheetAnim = desc.textureSheetAnimation;
    texSheetAnim.enabled = true;
    texSheetAnim.tileX = 13;
    texSheetAnim.tileY = 1;
    texSheetAnim.startFrame = 0;
    texSheetAnim.frameCount = 13;
    texSheetAnim.timeMode = ParticleSystemData::TimeMode::Lifetime;

    auto& renderer = desc.rendererModule;
    particleSystem->SetTextureResource(TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\fuwa_effect_sheet.png"));
    renderer.uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    renderer.billboardMode = ParticleSystemData::BillboardMode::View;
    renderer.blendMode = ParticleSystemData::BlendMode::AlphaBlend;
    renderer.sortByDistance = true;

    return dustEffect;
}

GameObject* RenderEffectFactory::CreateChargeAbsorbParticle(SceneBase* scene, const XMFLOAT3& position)
{
    if (!scene) return nullptr;

    GameObject* chargeEffect = scene->CreateGameObject();
    chargeEffect->SetName("ChargeAbsorbParticle");
    chargeEffect->SetRenderLayer(RenderLayer::Particle);

    TransformComponent* transform = chargeEffect->AddComponent<TransformComponent>();
    ParticleSystemComponent* particleSystem = chargeEffect->AddComponent<ParticleSystemComponent>();

    transform->SetPosition(position);

    auto& desc = particleSystem->GetDesc();
    auto& main = desc.mainModule;
    main.duration = 1.0f;
    main.loop = true;
    main.playOnAwake = true;
    main.startLifetime = { true, 0.15f, 0.05f, 0.35f };
    main.startSpeed = { true, -10.5f, -12.5f, -7.8f };
    main.startSize = { true, 1.22f, 1.08f, 1.32f };
    main.startColor.randomBetweenTwoColors = true;
    main.startColor.colorMin = { 0.25f, 0.75f, 1.0f, 0.85f };
    main.startColor.colorMax = { 0.95f, 0.35f, 1.0f, 0.55f };
    main.gravity = { 0.0f, 0.0f, 0.0f };
    main.simulationSpeed = 1.0f;
    main.simulationSpace = ParticleSystemData::SimulationSpace::Local;

    auto& emission = desc.emissionModule;
    emission.enabled = true;
    emission.rateOverTime = 60.0f;
    emission.rateOverDistance = 0.0f;

    auto& shape = desc.shapeModule;
    shape.enabled = true;
    shape.type = ParticleSystemData::ShapeType::Sphere;
    shape.sphere.radius = 1.5f;
    shape.sphere.emitFromShell = true;
    shape.randomDirectionAmount = 0.08f;

    auto& sizeOverLifetime = desc.sizeOverLifetimeModule;
    sizeOverLifetime.enabled = true;
    sizeOverLifetime.size.keys = {
        { 0.0f, 0.0f },
        { 0.12f, 1.0f },
        { 0.75f, 0.75f },
        { 1.0f, 0.0f },
    };

    auto& texSheetAnim = desc.textureSheetAnimation;
    texSheetAnim.enabled = true;
    texSheetAnim.tileX = 11;
    texSheetAnim.tileY = 1;
    texSheetAnim.startFrame = 0;
    texSheetAnim.frameCount = 11;
    texSheetAnim.timeMode = ParticleSystemData::TimeMode::Lifetime;

    auto& renderer = desc.rendererModule;
    particleSystem->SetTextureResource(TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\pika_effect_sheet.png"));
    renderer.uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    renderer.billboardMode = ParticleSystemData::BillboardMode::View;
    renderer.blendMode = ParticleSystemData::BlendMode::Additive;
    renderer.sortByDistance = true;

    return chargeEffect;
}

GameObject* RenderEffectFactory::CreateHitEffect(IScene* scene, const XMFLOAT3& position)
{
    if (!scene) return nullptr;

    GameObject* hitEffect = scene->CreateGameObject();
    hitEffect->SetName("HitEffect");
    hitEffect->SetRenderLayer(RenderLayer::Particle);

    TransformComponent* transform = hitEffect->AddComponent<TransformComponent>();
    ParticleSystemComponent* particleSystem = hitEffect->AddComponent<ParticleSystemComponent>();

    transform->SetPosition(position);

    auto& desc = particleSystem->GetDesc();
    auto& main = desc.mainModule;
    main.duration = 0.12f;
    main.loop = false;
    main.playOnAwake = true;
    main.startLifetime = { true, 0.22f, 0.12f, 0.34f };
    main.startSpeed = { true, 5.5f, 3.2f, 8.0f };
    main.startSize = { true, 0.18f, 0.08f, 0.28f };
    main.startColor.randomBetweenTwoColors = true;
    main.startColor.colorMin = { 0.45f, 0.85f, 1.0f, 0.95f };
    main.startColor.colorMax = { 1.0f, 0.95f, 0.35f, 0.75f };
    main.gravity = { 0.0f, -0.2f, 0.0f };
    main.simulationSpeed = 1.0f;
    main.simulationSpace = ParticleSystemData::SimulationSpace::World;

    auto& emission = desc.emissionModule;
    emission.enabled = true;
    emission.rateOverTime = 320.0f;
    emission.rateOverDistance = 0.0f;

    auto& shape = desc.shapeModule;
    shape.enabled = true;
    shape.type = ParticleSystemData::ShapeType::Sphere;
    shape.sphere.radius = 0.12f;
    shape.sphere.emitFromShell = false;
    shape.randomDirectionAmount = 0.9f;

    auto& sizeOverLifetime = desc.sizeOverLifetimeModule;
    sizeOverLifetime.enabled = true;
    sizeOverLifetime.size.keys = {
        { 0.0f, 0.25f },
        { 0.18f, 1.0f },
        { 1.0f, 0.0f },
    };

    auto& renderer = desc.rendererModule;
    particleSystem->SetTextureResource(TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\white.bmp"));
    renderer.uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    renderer.billboardMode = ParticleSystemData::BillboardMode::View;
    renderer.blendMode = ParticleSystemData::BlendMode::Additive;
    renderer.sortByDistance = true;

    return hitEffect;
}

GameObject* RenderEffectFactory::CreateExplosionEffect(IScene* scene, const XMFLOAT3& position)
{
    if (!scene) return nullptr;

    GameObject* explosionEffect = scene->CreateGameObject();
    explosionEffect->SetName("ExplosionEffect");
    explosionEffect->SetRenderLayer(RenderLayer::Particle);

    TransformComponent* transform = explosionEffect->AddComponent<TransformComponent>();
    ParticleSystemComponent* particleSystem = explosionEffect->AddComponent<ParticleSystemComponent>();

    transform->SetPosition(position);

    auto& desc = particleSystem->GetDesc();
    auto& main = desc.mainModule;
    main.duration = 0.22f;
    main.loop = false;
    main.playOnAwake = true;
    main.startLifetime = { true, 0.45f, 0.25f, 0.75f };
    main.startSpeed = { true, 7.5f, 4.5f, 12.0f };
    main.startSize = { true, 0.34f, 0.16f, 0.62f };
    main.startColor.randomBetweenTwoColors = true;
    main.startColor.colorMin = { 0.2f, 0.7f, 1.0f, 0.95f };
    main.startColor.colorMax = { 1.0f, 0.35f, 0.9f, 0.7f };
    main.gravity = { 0.0f, -0.35f, 0.0f };
    main.simulationSpeed = 1.0f;
    main.simulationSpace = ParticleSystemData::SimulationSpace::World;

    auto& emission = desc.emissionModule;
    emission.enabled = true;
    emission.rateOverTime = 820.0f;
    emission.rateOverDistance = 0.0f;

    auto& shape = desc.shapeModule;
    shape.enabled = true;
    shape.type = ParticleSystemData::ShapeType::Sphere;
    shape.sphere.radius = 0.45f;
    shape.sphere.emitFromShell = false;
    shape.randomDirectionAmount = 1.0f;

    auto& sizeOverLifetime = desc.sizeOverLifetimeModule;
    sizeOverLifetime.enabled = true;
    sizeOverLifetime.size.keys = {
        { 0.0f, 0.15f },
        { 0.12f, 1.25f },
        { 0.45f, 0.85f },
        { 1.0f, 0.0f },
    };

    auto& renderer = desc.rendererModule;
    particleSystem->SetTextureResource(TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\white.bmp"));
    renderer.uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    renderer.billboardMode = ParticleSystemData::BillboardMode::View;
    renderer.blendMode = ParticleSystemData::BlendMode::Additive;
    renderer.sortByDistance = true;

    return explosionEffect;
}
