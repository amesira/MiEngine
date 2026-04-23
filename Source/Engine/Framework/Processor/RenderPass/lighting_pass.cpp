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

#include "Utility/debug_ostream.h"

#include "Engine/engine_service_locator.h"

void LightingPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // 定数バッファの作成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.ByteWidth = sizeof(LightBufferData);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    m_pDevice->CreateBuffer(&buffer_desc, nullptr, m_lightBuffer.GetAddressOf());

    // ライティング全体を有効にしておく
    m_lightBufferData.enableLighting = 1;

    // シェーダーにライトバッファを登録
    auto* shader = EngineServiceLocator::GetShaderManager();
    shader->RegisterCB(ShaderManager::ShaderType::Lit, 10, m_lightBuffer.GetAddressOf());
    shader->RegisterCB(ShaderManager::ShaderType::SkinnedLit, 10, m_lightBuffer.GetAddressOf());
}

void LightingPass::Finalize()
{

}

void LightingPass::Process(IScene* pScene)
{
    auto* lightCompPool = pScene->GetComponentPool<LightComponent>();
    auto* transformCompPool = pScene->GetComponentPool<TransformComponent>();
    if (!lightCompPool || !transformCompPool) return;
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

    // 定数バッファにライトの情報を転送
    m_pContext->UpdateSubresource(m_lightBuffer.Get(), 0, nullptr, &m_lightBufferData, 0, 0);
}

// ライトのバインド
void LightingPass::BindLightCB(bool enable)
{
    m_lightBufferData.enableLighting = enable ? 1 : 0;

    // 定数バッファにライトの情報を転送
    m_pContext->UpdateSubresource(m_lightBuffer.Get(), 0, nullptr, &m_lightBufferData, 0, 0);
}
