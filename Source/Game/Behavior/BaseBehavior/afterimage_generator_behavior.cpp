//===================================================
// afterimage_generator_behavior.cpp
//===================================================
#include "afterimage_generator_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Graphics/material_repository.h"
#include "Engine/Graphics/shader_repository.h"
#include "Engine/Graphics/texture_repository.h"
#include "Engine/engine_service_locator.h"

#include "Game/Behavior/BaseBehavior/afterimage_behavior.h"

#include "External/ImGui/imgui.h"

#include <algorithm>

#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

namespace
{
    constexpr const char* AFTERIMAGE_SHADER_NAME = "AfterimageUnlit";
    constexpr const char* AFTERIMAGE_PIXEL_SHADER_PATH = "afterimage_unlit_ps.cso";
    constexpr const char* AFTERIMAGE_MATERIAL_NAME = "AfterimageMaterial";
    constexpr const wchar_t* DEFAULT_DITHER_TEXTURE_PATH = L"asset\\Texture\\Dither.png";
}

void AfterimageGeneratorBehavior::Start()
{
    GameObject* owner = GetOwner();
    if (!owner) return;

    if (!m_sourceTransform) {
        m_sourceTransform = owner->GetComponent<TransformComponent>();
    }
    if (!m_sourceSpriteRenderer) {
        m_sourceSpriteRenderer = owner->GetComponent<SpriteRendererComponent>();
    }

    m_afterimageMaterial = GetOrCreateAfterimageMaterial();
}

void AfterimageGeneratorBehavior::Update()
{
    if (!m_isEmitting) return;

    const float deltaTime = FPS_GetDeltaTime();
    m_emitTimer += deltaTime;

    if (m_emitTimer >= m_emitInterval) {
        m_emitTimer = 0.0f;
        EmitOnce();
    }
}

void AfterimageGeneratorBehavior::DrawComponentInspector()
{
    ImGui::Checkbox("Emitting", &m_isEmitting);
    ImGui::DragFloat("Emit Interval", &m_emitInterval, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat("Afterimage Life Time", &m_afterimageLifeTime, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Start Alpha", &m_startAlpha, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("End Alpha", &m_endAlpha, 0.01f, 0.0f, 1.0f);

    if (ImGui::Button("Emit Once")) {
        EmitOnce();
    }
}

void AfterimageGeneratorBehavior::SetSource(TransformComponent* transform, SpriteRendererComponent* spriteRenderer)
{
    m_sourceTransform = transform;
    m_sourceSpriteRenderer = spriteRenderer;
}

void AfterimageGeneratorBehavior::StartEmission()
{
    m_isEmitting = true;
    m_emitTimer = m_emitInterval;
}

void AfterimageGeneratorBehavior::StopEmission()
{
    m_isEmitting = false;
    m_emitTimer = 0.0f;
}

void AfterimageGeneratorBehavior::EmitOnce()
{
    if (!m_sourceTransform || !m_sourceSpriteRenderer) return;

    GameObject* owner = GetOwner();
    if (!owner || !owner->GetScene()) return;

    MaterialResource* afterimageMaterial = m_afterimageMaterial ? m_afterimageMaterial : GetOrCreateAfterimageMaterial();
    if (!afterimageMaterial) return;

    GameObject* afterimage = owner->GetScene()->CreateGameObject();
    afterimage->SetName("Afterimage");
    afterimage->SetRenderLayer(owner->GetRenderLayer());

    TransformComponent* transform = afterimage->AddComponent<TransformComponent>();
    SpriteRendererComponent* spriteRenderer = afterimage->AddComponent<SpriteRendererComponent>();
    AfterimageBehavior* behavior = afterimage->AddComponent<AfterimageBehavior>();

    transform->SetPosition(m_sourceTransform->GetPosition());
    transform->SetRotation(m_sourceTransform->GetRotation());
    transform->SetScaling(m_sourceTransform->GetScaling());

    spriteRenderer->GetMaterial() = m_sourceSpriteRenderer->GetMaterial();
    spriteRenderer->SetMaterialResource(afterimageMaterial);
    spriteRenderer->SetTextureResource(m_sourceSpriteRenderer->GetTextureResource());
    spriteRenderer->SetUvRect(m_sourceSpriteRenderer->GetUvRect());
    spriteRenderer->SetFlipX(m_sourceSpriteRenderer->GetFlipX());
    spriteRenderer->SetFlipY(m_sourceSpriteRenderer->GetFlipY());
    spriteRenderer->SetBlendMode(SpriteRendererComponent::SpriteBlendMode::Opaque);

    XMFLOAT4 color = m_sourceSpriteRenderer->GetColor();
    color.w = m_startAlpha;
    spriteRenderer->SetColor(color);

    behavior->Initialize(m_afterimageLifeTime, m_startAlpha, m_endAlpha);
}

void AfterimageGeneratorBehavior::SetAlphaRange(float startAlpha, float endAlpha)
{
    m_startAlpha = (std::clamp)(startAlpha, 0.0f, 1.0f);
    m_endAlpha = (std::clamp)(endAlpha, 0.0f, 1.0f);
}

MaterialResource* AfterimageGeneratorBehavior::GetOrCreateAfterimageMaterial()
{
    if (m_afterimageMaterial) return m_afterimageMaterial;
    if (!MATERIAL_REPOSITORY || !SHADER_REPOSITORY) return nullptr;

    ShaderProgramResource* shader = SHADER_REPOSITORY->GetShaderProgramResource(AFTERIMAGE_SHADER_NAME);
    if (!shader) {
        ShaderProgramResource shaderResource = {};
        shaderResource.name = AFTERIMAGE_SHADER_NAME;
        shaderResource.baseShader = SHADER_REPOSITORY->GetShaderProgramResource(ShaderBase::SpriteUnlit);
        shaderResource.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource(AFTERIMAGE_PIXEL_SHADER_PATH);

        if (shaderResource.baseShader && shaderResource.overridePixelShader) {
            shader = SHADER_REPOSITORY->GenerateShaderProgramResource(shaderResource);
        }
    }
    if (!shader) return nullptr;

    MaterialResource material = {};
    material.name = AFTERIMAGE_MATERIAL_NAME;
    material.renderMode = RenderMode::Opaque;
    material.shaderProgram = shader;
    material.baseColor = { 0.55f, 0.85f, 1.0f, 1.0f };
    material.emissiveColor = { 0.2f, 0.65f, 1.0f };
    material.emissiveIntensity = 1.0f;

    if (TEXTURE_REPOSITORY) {
        material.customTextures[0] = TEXTURE_REPOSITORY->GetTextureResource(DEFAULT_DITHER_TEXTURE_PATH);
    }

    m_afterimageMaterial = MATERIAL_REPOSITORY->GenerateMaterial(material);
    return m_afterimageMaterial;
}
