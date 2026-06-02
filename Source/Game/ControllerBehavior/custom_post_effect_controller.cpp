//===================================================
// custom_post_effect_controller.cpp
// 
// Author: Miu Kitamura
// Date  : 2026/06/02
//===================================================
#include "custom_post_effect_controller.h"

#include <algorithm>

#include "Engine/Device/mi_fps.h"

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
    float deltaTime = FPS_GetUnscaledDeltaTime();

    // 各エフェクトのTweeningタスクを更新し、状態に反映させる
    for (int i = 0; i < static_cast<int>(CustomPostEffectType::MAX); i++) {
        FloatTweenTask& tweenTask = m_changeIntensityTask[i];
        bool isRunning = !tweenTask.IsFinished();

        tweenTask.Update(deltaTime);

        if (isRunning) {
            switch (static_cast<CustomPostEffectType>(i)) {
            case CustomPostEffectType::RadialBlur:
                m_state.radialBlur.strength = tweenTask.m_currentValue;
                break;
            case CustomPostEffectType::MonoMask:
                m_state.monoMask.strength = tweenTask.m_currentValue;
                break;
            default: break;
            }
        }
    }
}

void CustomPostEffectController::DrawComponentInspector()
{

}

// ポストエフェクトの再生
void CustomPostEffectController::PlayEffect(
    CustomPostEffectType effectType, 
    float intensity, float duration, float holdDuration)
{
    int effectIndex = static_cast<int>(effectType);

    FloatTweenTask& tweenTask = m_changeIntensityTask[effectIndex];
    tweenTask.Reset();
    tweenTask.m_duration = duration;
    tweenTask.m_holdDuration = holdDuration;
    tweenTask.m_targetValue = intensity;
    tweenTask.m_endValue = 0.0f; // 終了後は強度0に戻す

    // 開始値は現在のエフェクトの強度にする
    switch (effectType) {
        case CustomPostEffectType::RadialBlur:
            tweenTask.m_startValue = m_state.radialBlur.strength;
            break;
        case CustomPostEffectType::MonoMask:
            tweenTask.m_startValue = m_state.monoMask.strength;
            break;
        default: break;
    }

    tweenTask.Start();
}
