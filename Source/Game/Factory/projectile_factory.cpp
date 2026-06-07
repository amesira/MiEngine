//===================================================
// projectile_factory.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/07
//===================================================
#include "projectile_factory.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"

#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/particle_system_component.h"
#include "Engine/Framework/Component/transform_component.h"

#include "Game/Behavior/BaseBehavior/bullet_behavior.h"
#include "Game/Behavior/BaseBehavior/hit_stop_behavior.h"

#include "Engine/Graphics/material_repository.h"
#include "Engine/Graphics/model_repository.h"
#include "Engine/Graphics/shader_repository.h"
#include "Engine/Graphics/texture_repository.h"

#include "Engine/engine_service_locator.h"

#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define MODEL_REPOSITORY EngineServiceLocator::GetModelRepository()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

namespace
{
    // ホログラムシェーダーの取得または生成
    ShaderProgramResource* GetOrCreateHologramShader(const ProjectileFactory::BulletCreateDesc& desc)
    {
        if (!SHADER_REPOSITORY) return nullptr;
        const char* hologramShaderName = "HologramUnlit";
        const char* hologramPixelShaderPath = "hologram_unlit_ps.cso";

        if (ShaderProgramResource* shader = SHADER_REPOSITORY->GetShaderProgramResource(hologramShaderName)) {
            return shader;
        }

        ShaderProgramResource shaderResource = {};
        shaderResource.name = hologramShaderName;
        shaderResource.baseShader = SHADER_REPOSITORY->GetShaderProgramResource(ShaderBase::Unlit);
        shaderResource.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource(hologramPixelShaderPath);

        if (!shaderResource.baseShader || !shaderResource.overridePixelShader) return nullptr;

        return SHADER_REPOSITORY->GenerateShaderProgramResource(shaderResource);
    }

    // 弾のマテリアルの生成
    MaterialResource* CreateBulletMaterial(const ProjectileFactory::BulletCreateDesc& desc)
    {
        if (!MATERIAL_REPOSITORY) return nullptr;
        static bool generated = false;

        if (generated) {
            MaterialResource* existingMaterial = MATERIAL_REPOSITORY->GetMaterial(desc.materialName);
            if (existingMaterial) {
                return existingMaterial;
            }
        }
        generated = true;
        
        XMFLOAT4 hologramColor = { 0.35f, 0.85f, 1.0f, 0.75f };
        float hologramIntensity = 7.0f;

        MaterialResource material = {};
        material.name = desc.materialName;
        material.renderMode = RenderMode::Opaque;
        material.shaderProgram = GetOrCreateHologramShader(desc);
        material.baseColor = hologramColor;
        material.emissiveColor = { hologramColor.x, hologramColor.y, hologramColor.z };
        material.emissiveIntensity = hologramIntensity;

        material.customProperties[0] = hologramColor;
        material.customProperties[1] = { hologramIntensity, 0.0f, 0.0f, 0.0f };

        if (TEXTURE_REPOSITORY) {
            material.customTextures[0] = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\hologram_noise.png");
            material.customTextures[1] = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\Dither.png");
        }

        return MATERIAL_REPOSITORY->GenerateMaterial(material);
    }

    // モデルコンポーネントのセットアップ
    void SetupBulletModel(ModelComponent* modelComponent, const ProjectileFactory::BulletCreateDesc& desc)
    {
        if (!modelComponent || !MODEL_REPOSITORY) return;

        ModelResource* modelResource = MODEL_REPOSITORY->GetModel(desc.modelPath);
        if (!modelResource) return;
        modelComponent->SetModelResource(modelResource);

        // ホログラムマテリアルの生成と適用
        MaterialResource* bulletMaterial = CreateBulletMaterial(desc);
        if (!bulletMaterial) return;

        // モデルの全マテリアルスロットにホログラムマテリアルを適用
        for (MaterialInstance& materialSlot : modelComponent->GetMaterialSlots()) {
            materialSlot.materialResource = bulletMaterial;
        }
    }

    // パーティクルコンポーネントのセットアップ
    void SetupBulletParticle(ParticleSystemComponent* particleSystem, const ProjectileFactory::BulletCreateDesc& desc)
    {
        if (!particleSystem) return;

        particleSystem->Main().loop = true;
        particleSystem->Main().playOnAwake = true;
        particleSystem->Main().startLifetime = { false, 0.2f, 0.2f, 0.2f };
        particleSystem->Main().startSpeed = { false, 0.0f, 0.0f, 0.0f };
        particleSystem->Main().startSize = { false, desc.radius * 0.8f, desc.radius * 0.8f, desc.radius * 0.8f };
        particleSystem->Main().startColor = { false, {0.6f, 0.2f, 0.3f, 0.5f}, {0.4f, 0.5f, 0.5f, 0.8f}};

        particleSystem->Emission().enabled = true;
        particleSystem->Emission().rateOverTime = 30.0f;
        particleSystem->Emission().rateOverDistance = 0.0f;

        particleSystem->Shape().enabled = true;
        particleSystem->Shape().type = ParticleSystemComponent::ShapeType::Sphere;
        particleSystem->Shape().sphere.radius = desc.radius;
        particleSystem->Shape().sphere.emitFromShell = true;
        particleSystem->Shape().randomDirectionAmount = 1.0f;

        particleSystem->SizeOverLifetime().enabled = true;
        particleSystem->SizeOverLifetime().size.keys = {
            { 0.0f, 1.0f },
            { 1.0f, 0.0f },
        };

        particleSystem->Renderer().blendMode = ParticleSystemComponent::BlendMode::Additive;
        particleSystem->Renderer().billboardMode = ParticleSystemComponent::BillboardMode::View;

        if (TEXTURE_REPOSITORY) {
            particleSystem->Renderer().textureResource = TEXTURE_REPOSITORY->GetTextureResource(L"asset/Texture/white.bmp");
        }
    }
}

// 弾の生成
GameObject* ProjectileFactory::CreateBullet(IScene* scene, const BulletCreateDesc& desc)
{
    if (!scene) return nullptr;

    GameObject* bullet = scene->CreateGameObject();
    bullet->SetName("Bullet");
    bullet->SetRenderLayer(RenderLayer::Bullet);

    TransformComponent* transform = bullet->AddComponent<TransformComponent>();
    ModelComponent* modelComponent = bullet->AddComponent<ModelComponent>();
    ParticleSystemComponent* particleSystem = bullet->AddComponent<ParticleSystemComponent>();
    BulletBehavior* bulletBehavior = bullet->AddComponent<BulletBehavior>();
    HitStopBehavior* hitStopBehavior = bullet->AddComponent<HitStopBehavior>();

    transform->SetPosition(desc.position);
    transform->SetScaling({ desc.radius * 2.0f, desc.radius * 2.0f, desc.radius * 2.0f });

    // 弾の進行方向に合わせて回転を設定
    XMFLOAT3 forward = MiMath::Multiply(desc.velocity, -1.0f);
    MiMath::Normalize(forward);
    XMFLOAT4 rotation = MiMath::QuaternionFromDirection(forward, { 0.0f, 1.0f, 0.0f });
    transform->SetRotation(rotation);

    SetupBulletModel(modelComponent, desc);
    SetupBulletParticle(particleSystem, desc);

    bulletBehavior->Initialize(desc.velocity, desc.radius, desc.lifeTime, desc.layerMask);

    return bullet;
}
