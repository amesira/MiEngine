//===================================================
// custom_post_effect_controller.cpp
// 
// Author: Miu Kitamura
// Date  : 2026/06/02
//===================================================
#include "custom_post_effect_controller.h"

#include <algorithm>

#include "game_controller_locator.h"
#include "Utility/debug_ostream.h"

CustomPostEffectController::CustomPostEffectController()
{
    s_instanceCount++;

    if (s_instanceCount > 1) {
        hal::dout << "Warning: CustomPostEffectController has multiple instances. Only one instance is expected." << std::endl;
        this->SetEnable(false);
    }
    else {
        GameControllerLocator::s_customPostEffectController = this;
    }

    m_state.Reset();
}

CustomPostEffectController::~CustomPostEffectController()
{
    if (GameControllerLocator::s_customPostEffectController == this) {
        GameControllerLocator::s_customPostEffectController = nullptr;
    }

    s_instanceCount--;
}

void CustomPostEffectController::Start()
{
}

void CustomPostEffectController::Update()
{
}

void CustomPostEffectController::DrawComponentInspector()
{
}

void CustomPostEffectController::SetRadialBlurIntensity(float intensity)
{
    m_state.radialBlurIntensity = (std::max)(0.0f, intensity);
}

void CustomPostEffectController::SetMonochromeIntensity(float intensity)
{
    m_state.monochromeIntensity = (std::max)(0.0f, intensity);
}

void CustomPostEffectController::SetMaskIntensity(float intensity)
{
    m_state.maskIntensity = (std::max)(0.0f, intensity);
}

void CustomPostEffectController::SetMaskTexture(ID3D11ShaderResourceView* maskSRV)
{
    m_state.maskSRV = maskSRV;
}

void CustomPostEffectController::ResetPostEffect()
{
    m_state.Reset();
}

CustomPostEffectState CustomPostEffectController::GetActiveState() const
{
    if (!m_enabled) {
        CustomPostEffectState inactiveState;
        inactiveState.Reset();
        return inactiveState;
    }

    return m_state;
}

bool CustomPostEffectController::IsActive() const
{
    return m_enabled && m_state.IsActive();
}
