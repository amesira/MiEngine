//---------------------------------------------------
// custom_post_effect_controller.h
// 
// Author: Miu Kitamura
// Date  : 2026/06/02
//---------------------------------------------------
#ifndef CUSTOM_POST_EFFECT_CONTROLLER_H
#define CUSTOM_POST_EFFECT_CONTROLLER_H
#include "Engine/Framework/Component/behavior_component.h"
#include "Engine/Framework/Processor/RenderPass/PostEffect/custom_post_effect.h"

#include "Engine/Core/GamePlay/tween_task.h"

#include "Engine/Settings/post_process_settings.h"

// カスタムポストエフェクトの種類
enum class CustomPostEffectType {
    RadialBlur,
    MonoMask,

    MAX,
};

class CustomPostEffectController : public BehaviorComponent {
private:
    static inline int s_instanceCount = 0;

    // ポストエフェクトの状態
    CustomPostEffectState m_state = {};

    // ポストエフェクトの状態をTweeningするタスク
    FloatTweenTask m_changeIntensityTask[static_cast<int>(CustomPostEffectType::MAX)];

public:
    CustomPostEffectController();
    ~CustomPostEffectController();

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // ポストエフェクトの再生
    void PlayEffect(CustomPostEffectType effectType, float intensity, float duration, float holdDuration);

    // RadialBlurサンプル数設定
    void SetRadialBlur_SampleCount(int count) { m_state.radialBlur.sampleCount = count; }
    // MonoMask色設定
    void SetMonoMask_MonoColor(XMFLOAT4 color) { m_state.monoMask.monoColor = color; }
    // MonoMaskテクスチャ用テクスチャ設定
    void SetMonoMask_Mask(ID3D11ShaderResourceView* srv) { m_state.monoMaskTextureSRV = srv; }

};

#endif // CUSTOM_POST_EFFECT_CONTROLLER_H
