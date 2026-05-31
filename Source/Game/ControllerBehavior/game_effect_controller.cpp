//===================================================
// game_effect_controller.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/27
//===================================================
#include "game_effect_controller.h"

#include "Engine/Device/mi_fps.h"
#include "game_controller_locator.h"

#include "Utility/mi_math.h"

#include "Utility/debug_ostream.h"

GameEffectController::GameEffectController()
{
    s_instanceCount++;

    if (s_instanceCount > 1) {
        hal::dout << "警告: GameEffectControllerのインスタンスが複数存在しています。ゲーム全体で1つだけ存在することを想定しています。" << std::endl;
        this->SetEnable(false);
    }
    else {
        GameControllerLocator::s_gameEffectController = this;
    }

    // タスクの初期化
    m_changeTimeScaleTask.Reset();
}

GameEffectController::~GameEffectController()
{
    if (GameControllerLocator::s_gameEffectController == this) {
        GameControllerLocator::s_gameEffectController = nullptr;
    }

    s_instanceCount--;
}

void GameEffectController::Start()
{
    
}

void GameEffectController::Update()
{
    float unscaledDeltaTime = FPS_GetUnscaledDeltaTime();

    // タスクの更新
    m_changeTimeScaleTask.Update(unscaledDeltaTime);
}

void GameEffectController::DrawComponentInspector()
{

}

//------------------------------- private

// タイムスケール変更タスクの更新
void GameEffectController::ChangeTimeScaleTask::Update(float deltaTime)
{
    if (!m_isRunning) return;
    SequenceTask::Update(deltaTime);

    switch(m_taskStep) {
        case 0: {
            // タイムスケールを目標値に向けて変化させる
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }
            float newTimeScale = MiMath::Lerp(m_startTimeScale, m_targetTimeScale, t);
            FPS_SetTimeScale(newTimeScale);

            if (t >= 1.0f) {
                AdvanceStep();
            }
            break;
        }
        case 1: {
            if (m_holdDuration > 0.0f) {
                // 一定時間保持
                if (Wait(m_holdDuration, deltaTime)) {
                    AdvanceStep();
                }
            }
            else {
                Finish(); // 保持なしで完了
            }
            break;
        }
        case 2: {
            // タイムスケールを元に戻す
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }
            float newTimeScale = MiMath::Lerp(m_targetTimeScale, m_startTimeScale, t);
            FPS_SetTimeScale(newTimeScale);

            if (t >= 1.0f) {
                FPS_SetTimeScale(m_startTimeScale); // 確実に元のタイムスケールに戻す
                Finish();
            }
            break;
        }
    }
}


//------------------------------- public

// タイムスケール変更
void GameEffectController::ChangeTimeScale(float timeScale, float duration)
{
    m_changeTimeScaleTask.Reset();

    m_changeTimeScaleTask.m_startTimeScale = FPS_GetTimeScale();
    m_changeTimeScaleTask.m_targetTimeScale = timeScale;
    m_changeTimeScaleTask.m_duration = duration;
    m_changeTimeScaleTask.m_holdDuration = 0.0f;
    m_changeTimeScaleTask.Start();
}
void GameEffectController::ChangeTimeScaleTemporary(float timeScale, float duration, float holdDuration)
{
    m_changeTimeScaleTask.Reset();

    m_changeTimeScaleTask.m_startTimeScale = FPS_GetTimeScale();
    m_changeTimeScaleTask.m_targetTimeScale = timeScale;
    m_changeTimeScaleTask.m_duration = duration;
    m_changeTimeScaleTask.m_holdDuration = holdDuration;
    m_changeTimeScaleTask.Start();
}
// タイムスケールを元に戻す
void GameEffectController::ResetTimeScale(float duration)
{
    ChangeTimeScale(1.0f, duration);
}
