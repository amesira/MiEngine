//===================================================
// game_effect_controller.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/27
//===================================================
#include "game_effect_controller.h"

#include "Engine/System/Device/mi_fps.h"
#include "game_controller_locator.h"

#include "Utility/debug_ostream.h"

void GameEffectController::Start()
{
    if (s_instanceCount > 1) {
        hal::dout << "警告: GameEffectControllerのインスタンスが複数存在しています。ゲーム全体で1つだけ存在することを想定しています。" << std::endl;
        this->SetEnable(false);
    }
    else {
        GameControllerLocator::s_gameEffectController = this;
    }
}

void GameEffectController::Update()
{
    float unscaledDeltaTime = FPS_GetUnscaledDeltaTime();

    // TimeScale変更の継続時間を更新
    if (m_timeScaleChangeTimer < m_timeScaleChangeDuration) {
        m_timeScaleChangeTimer += unscaledDeltaTime;

        if (m_timeScaleChangeTimer >= m_timeScaleChangeDuration) {
            m_currentTimeScale = 1.0f;
            m_timeScaleChangeDuration = 0.0f;
            m_timeScaleChangeTimer = 0.0f;
        }
    }

    // TimeScaleをゲーム全体に適用
    FPS_SetTimeScale(m_currentTimeScale);
}

void GameEffectController::DrawComponentInspector()
{

}

//------------------------------- public

// TimeScale変更のリクエスト処理 return : handleID
int GameEffectController::RequestTimeScaleChange(float targetTimeScale, float duration)
{
    m_currentTimeScale = targetTimeScale;

    m_timeScaleChangeDuration = duration;
    m_timeScaleChangeTimer = 0.0f;

    m_currentHandleID = m_handleIDCounter++;
    return m_currentHandleID;
}

// TimeScale変更のキャンセル処理
void GameEffectController::CancelTimeScaleChange(int handleID)
{
    if (handleID == m_currentHandleID) {
        m_currentTimeScale = 1.0f;
        m_timeScaleChangeDuration = 0.0f;
        m_timeScaleChangeTimer = 0.0f;
    }
}
