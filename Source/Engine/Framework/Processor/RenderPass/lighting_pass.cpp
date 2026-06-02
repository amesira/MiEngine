//===================================================
// light_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/01/05
//===================================================
#include "lighting_pass.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/light_component.h"

#include "Engine/Settings/scene_settings.h"

#include "Utility/debug_ostream.h"

#include "Engine/engine_service_locator.h"

#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

void LightingPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // ライト用の定数バッファ作成
    m_lightCB = SHADER_REPOSITORY->GenerateConstantBufferResource(
        "LightBuffer",
        10,
        sizeof(LightBufferData),
        true,
        true,
        ConstantBufferUsage::Default);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::Lit)], m_lightCB);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::SkinnedLit)], m_lightCB);

    // ライティング全体を有効にしておく
    m_lightBufferData.enableLighting = 1;
}

void LightingPass::Finalize()
{

}

void LightingPass::Process(IScene* pScene, const RenderView& view)
{
    (void)view;
    // Componentからライトの情報を転送
    auto* lightCompPool = pScene->GetComponentPool<LightComponent>();
    auto* transformCompPool = pScene->GetComponentPool<TransformComponent>();
    if (lightCompPool && transformCompPool) {
        CollectLightComponents(lightCompPool, transformCompPool);
    }

    // LightSettingsからライトの情報を転送
    const LightingSettings& lightingSettings = pScene->GetSceneSettings().GetLightingSettings();
    CollectLightSettings(lightingSettings);

    // 定数バッファにライトの情報を転送
    m_pContext->UpdateSubresource(m_lightCB->buffer.Get(), 0, nullptr, &m_lightBufferData, 0, 0);
}

// ライトのバインド
void LightingPass::BindLightCB(bool enable)
{
    m_lightBufferData.enableLighting = enable ? 1 : 0;

    // 定数バッファにライトの情報を転送
    m_pContext->UpdateSubresource(m_lightCB->buffer.Get(), 0, nullptr, &m_lightBufferData, 0, 0);
}

// ------------------------------------ private

// Componentからライトの情報を転送
void LightingPass::CollectLightComponents(ComponentPool<LightComponent>* lightCompPool, ComponentPool<TransformComponent>* transformCompPool)
{
    auto& lightCompList = lightCompPool->GetList();

    // ライトの数をリセット
    for (int i = 0; i < DIRECTIONAL_LIGHT_MAX; i++) {
        m_lightBufferData.directionalLights[i].enable = 0;
    }
    for (int i = 0; i < POINT_LIGHT_MAX; i++) {
        m_lightBufferData.pointLights[i].enable = 0;
    }
    for (int i = 0; i < SPOT_LIGHT_MAX; i++) {
        m_lightBufferData.spotLights[i].enable = 0;
    }

    int directionalLightCount = 0;
    int pointLightCount = 0;
    int spotLightCount = 0;

    // LightComponentPoolを走査
    for (LightComponent& lightComp : lightCompList) {
        LightComponent* light = &lightComp;
        TransformComponent* transform = transformCompPool->GetByGameObjectID(lightComp.GetOwner()->GetID());
        if (!light->GetEnable() || !transform) continue;
        if (!light->GetOwner()->GetActive()) continue;

        // ライトの情報をバッファデータに設定
        switch (light->GetLightType()) {
        case LightComponent::LightType::Directional:
        {
            if (directionalLightCount >= DIRECTIONAL_LIGHT_MAX) continue;
            GPU_DirectionalLight& data = m_lightBufferData.directionalLights[directionalLightCount];
            {
                data.enable = 1;
                data.direction = light->GetDirection();
                data.diffuse = light->GetDiffuse();
                data.ambient = light->GetAmbient();

                data.intensity = light->GetIntensity();
            }
            directionalLightCount++;
            break;
        }
        case LightComponent::LightType::Point:
        {
            if (pointLightCount >= POINT_LIGHT_MAX) continue;
            GPU_PointLight& data = m_lightBufferData.pointLights[pointLightCount];
            {
                data.enable = 1;
                data.position = {
                    transform->GetPosition().x,
                    transform->GetPosition().y,
                    transform->GetPosition().z,
                    1.0f
                };
                data.diffuse = light->GetDiffuse();

                data.intensity = light->GetIntensity();
                data.range = light->GetRange();
            }
            pointLightCount++;
            break;
        }
        case LightComponent::LightType::Spot:
        {
            if (spotLightCount >= SPOT_LIGHT_MAX) continue;
            GPU_SpotLight& data = m_lightBufferData.spotLights[spotLightCount];
            {
                data.enable = 1;
                data.position = {
                    transform->GetPosition().x,
                    transform->GetPosition().y,
                    transform->GetPosition().z,
                    1.0f
                };
                data.direction = light->GetDirection();
                data.diffuse = light->GetDiffuse();
                data.intensity = light->GetIntensity();
                data.range = light->GetRange();
                data.spotAngle = light->GetSpotAngle();
            }
            spotLightCount++;
            break;
        }
        }
    }
}


// LightSettingsからライトの情報を転送
void LightingPass::CollectLightSettings(const LightingSettings& lightingSettings)
{
    const RimLightSettings& rimLightSettings = lightingSettings.GetRimLightSettings();
    m_lightBufferData.rimLight.enable = rimLightSettings.enabled ? 1 : 0;
    m_lightBufferData.rimLight.intensity = rimLightSettings.intensity;
    m_lightBufferData.rimLight.threshold = rimLightSettings.threshold;
    m_lightBufferData.rimLight.color = XMFLOAT4(rimLightSettings.color.x, rimLightSettings.color.y, rimLightSettings.color.z, 1.0f);

    const HemisphereLightSettings& hemisphereLightSettings = lightingSettings.GetHemisphereLightSettings();
    m_lightBufferData.hemisphereLight.enable = hemisphereLightSettings.enabled ? 1 : 0;
    m_lightBufferData.hemisphereLight.intensity = hemisphereLightSettings.intensity;
    m_lightBufferData.hemisphereLight.skyColor = XMFLOAT4(hemisphereLightSettings.skyColor.x, hemisphereLightSettings.skyColor.y, hemisphereLightSettings.skyColor.z, 1.0f);
    m_lightBufferData.hemisphereLight.groundColor = XMFLOAT4(hemisphereLightSettings.groundColor.x, hemisphereLightSettings.groundColor.y, hemisphereLightSettings.groundColor.z, 1.0f);

}
