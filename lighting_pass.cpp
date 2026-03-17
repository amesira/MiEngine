//===================================================
// light_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/01/05
//===================================================
#include "lighting_pass.h"
#include "scene_interface.h"
#include "game_object.h"

#include "shader.h"
#include "transform_component.h"
#include "light_component.h"

#include "debug_ostream.h"

void LightingPass::Initialize()
{
    // 定数バッファの作成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.ByteWidth = sizeof(LightBufferData);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, m_lightBuffer.GetAddressOf());

    // ライティング全体を有効にしておく
    m_lightBufferData.enableLighting = 1;
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

    m_lightBufferData = {};

    // ライトの情報をバッファデータに設定
    for (LightComponent& lightComp : lightCompList) {
        LightComponent* light = &lightComp;
        TransformComponent* transform = transformCompPool->GetByGameObjectID(lightComp.GetOwner()->GetID());
        if (!light->GetEnable() || !transform) continue;

        // DirectionalLightとして設定（今回はDirectionalLightのみ対応）
        //m_lightBufferData.directionalLights[0].enable = 1;
        //m_lightBufferData.directionalLights[0].direction = light->GetDirection(); // ライトの方向を設定
        //m_lightBufferData.directionalLights[0].diffuse = light->GetDiffuse();
        //m_lightBufferData.directionalLights[0].ambient = light->GetAmbient();

        m_lightBufferData.pointLights[0].enable = 1;
        m_lightBufferData.pointLights[0].position = {
            transform->GetPosition().x,
            transform->GetPosition().y,
            transform->GetPosition().z,
            1.0f // w成分はポイントライトであることを示すために1.0fにする
        };
        m_lightBufferData.pointLights[0].diffuse = light->GetDiffuse();
        m_lightBufferData.pointLights[0].range = 10.0f; // ライトの範囲を設定
    }

    // 定数バッファにライトの情報を転送
    Direct3D_GetDeviceContext()->UpdateSubresource(m_lightBuffer.Get(), 0, nullptr, &m_lightBufferData, 0, 0);
}

// ライトバッファをシェーダーにバインド
void LightingPass::BindLightBuffer()
{
    Shader_BindVsConstantBuffer(10, m_lightBuffer.Get());
    Shader_BindPsConstantBuffer(10, m_lightBuffer.Get());
}

// ライトの有効・無効設定
void LightingPass::SetLightEnable(bool enable)
{
    m_lightBufferData.enableLighting = enable ? 1 : 0;

    // 定数バッファにライトの情報を転送
    Direct3D_GetDeviceContext()->UpdateSubresource(m_lightBuffer.Get(), 0, nullptr, &m_lightBufferData, 0, 0);
}
