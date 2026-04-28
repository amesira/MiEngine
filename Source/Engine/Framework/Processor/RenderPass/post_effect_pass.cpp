//===================================================
// post_effect_pass.cpp
//
// Author: Miu Kitamura
// Date  : 2026/04/29
//===================================================
#include "post_effect_pass.h"

#include "Engine/Core/scene_interface.h"

// PostEffectPass initial setup
void PostEffectPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
}

// PostEffectPass cleanup
void PostEffectPass::Finalize()
{
}

// PostEffectPass processing
void PostEffectPass::Process(IScene* pScene)
{
    (void)pScene;
}
