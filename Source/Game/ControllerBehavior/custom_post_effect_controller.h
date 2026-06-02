//---------------------------------------------------
// custom_post_effect_controller.h
// 
// Author: Miu Kitamura
// Date  : 2026/06/02
//---------------------------------------------------
#ifndef CUSTOM_POST_EFFECT_CONTROLLER_H
#define CUSTOM_POST_EFFECT_CONTROLLER_H

#include "Engine/Framework/Component/behavior_component.h"
#include "Engine/Framework/Processor/RenderPass/custom_post_effect_pass.h"

class CustomPostEffectController : public BehaviorComponent {
private:
    static inline int s_instanceCount = 0;

    bool m_enabled = true;
    CustomPostEffectState m_state = {};

public:
    CustomPostEffectController();
    ~CustomPostEffectController();

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    void SetPostEffectEnabled(bool enabled) { m_enabled = enabled; }
    bool IsPostEffectEnabled() const { return m_enabled; }

    void SetRadialBlurIntensity(float intensity);
    void SetMonochromeIntensity(float intensity);
    void SetMaskIntensity(float intensity);
    void SetMaskTexture(ID3D11ShaderResourceView* maskSRV);

    float GetRadialBlurIntensity() const { return m_state.radialBlurIntensity; }
    float GetMonochromeIntensity() const { return m_state.monochromeIntensity; }
    float GetMaskIntensity() const { return m_state.maskIntensity; }
    ID3D11ShaderResourceView* GetMaskTexture() const { return m_state.maskSRV; }

    void ResetPostEffect();

    const CustomPostEffectState& GetState() const { return m_state; }
    CustomPostEffectState GetActiveState() const;
    bool IsActive() const;

};

#endif // CUSTOM_POST_EFFECT_CONTROLLER_H
