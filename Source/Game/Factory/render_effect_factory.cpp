//===================================================
// render_effect_factory.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//===================================================
#include "render_effect_factory.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_base.h"

// component
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/decal_component.h"
#include "Engine/Framework/Component/particle_system_component.h"

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

    TransformComponent* transform = particleEffect->AddComponent<TransformComponent>();
    ParticleSystemComponent* particleSystem = particleEffect->AddComponent<ParticleSystemComponent>();

    transform->SetPosition(position);

    auto& main = particleSystem->Main();
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
    main.simulationSpace = ParticleSystemComponent::SimulationSpace::Local;

    auto& emission = particleSystem->Emission();
    emission.enabled = true;
    emission.rateOverTime = 24.0f;
    emission.rateOverDistance = 0.0f;

    auto& shape = particleSystem->Shape();
    shape.enabled = true;
    shape.type = ParticleSystemComponent::ShapeType::Cone;
    shape.cone.angle = XM_PIDIV4;
    shape.cone.radius = 0.35f;
    shape.cone.length = 2.0f;
    shape.cone.emitFromBase = true;
    shape.randomDirectionAmount = 0.2f;

    auto& sizeOverLifetime = particleSystem->SizeOverLifetime();
    sizeOverLifetime.enabled = true;
    sizeOverLifetime.size.keys = {
        { 0.0f, 0.35f },
        { 0.25f, 1.0f },
        { 1.0f, 0.0f },
    };

    auto& renderer = particleSystem->Renderer();
    renderer.textureResource = TEXTURE_REPOSITORY->GetTextureResource(texturePath);
    renderer.uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    renderer.billboardMode = ParticleSystemComponent::BillboardMode::View;
    renderer.blendMode = ParticleSystemComponent::BlendMode::Additive;
    renderer.sortByDistance = true;

    particleSystem->Play();
    return particleEffect;
}
