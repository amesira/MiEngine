//===================================================
// light_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/01/05
//===================================================
#include "lighting_pass.h"
#include "scene_interface.h"

#include "shader.h"
#include "light_component.h"

void LightingPass::Initialize()
{

}

void LightingPass::Finalize()
{

}

void LightingPass::Process(IScene* pScene)
{
    auto* lightCompPool = pScene->GetComponentPool<LightComponent>();
    if (!lightCompPool) return;

    auto& lightCompList = lightCompPool->GetList();

    // ライトの情報をシェーダーに設定
    m_lightCount = 0;
    for (LightComponent& light : lightCompList) {
        if (!light.GetEnable()) continue;

        Shader_SetLight(
            m_lightCount,
            light.GetDirection(),
            light.GetDiffuse(),
            light.GetAmbient()
        );

        m_lightCount++;
        if (m_lightCount >= MAX_LIGHT) break;
    }

    // 残りのライトを無効化
    for (int i = 0; i < MAX_LIGHT - m_lightCount; i++) {
        int index = m_lightCount + i;
        Shader_SetLightEnable(index, false);
    }
}

// ライトの有効・無効設定
void LightingPass::SetLightEnable(bool enable)
{
    for (int i = 0; i < m_lightCount; i++) {
        Shader_SetLightEnable(i, enable);
    }
}
