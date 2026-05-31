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
}

GameEffectController::~GameEffectController()
{
    if (GameControllerLocator::s_gameEffectController == this) {
        GameControllerLocator::s_gameEffectController = nullptr;
    }

    if (m_changeTimeScaleTask) {
        delete m_changeTimeScaleTask;
        m_changeTimeScaleTask = nullptr;
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
    if (m_changeTimeScaleTask && !m_changeTimeScaleTask->IsFinished()) {
        m_changeTimeScaleTask->Update(unscaledDeltaTime);
    }
    else if (m_changeTimeScaleTask) {
        delete m_changeTimeScaleTask;
        m_changeTimeScaleTask = nullptr;
    }

}

void GameEffectController::DrawComponentInspector()
{

}

//------------------------------- private

void GameEffectController::ChangeTimeScaleTask::Start()
{
    SequenceTask::Start();

    // タスク開始時に現在のタイムスケールを保存
    m_startTimeScale = FPS_GetTimeScale();
}

// タイムスケール変更タスクの更新
void GameEffectController::ChangeTimeScaleTask::Update(float deltaTime)
{
    if (!m_isRunning || m_isFinished) return;
    SequenceTask::Update(deltaTime);

    switch(m_taskStep) {
        case 0: {
            // タイムスケールを目標値に向けて変化させる
            float t = 1.0f;
            if (m_duration > 0.0f) (std::min)(m_taskTimer / m_duration, 1.0f);
            float newTimeScale = MiMath::Lerp(m_startTimeScale, m_targetTimeScale, t);
            FPS_SetTimeScale(newTimeScale);

            if (t >= 1.0f) {
                AdvanceStep();
            }
            break;
        }
        case 1: {
            if (m_isTemporary) {
                // 一定時間保持
                if (Wait(m_holdDuration, deltaTime)) {
                    AdvanceStep();
                }
            }
            else {
                m_isFinished = true; // タスク完了
            }
            break;
        }
        case 2: {
            // タイムスケールを元に戻す
            float t = 1.0f;
            if (m_duration > 0.0f) (std::min)(m_taskTimer / m_duration, 1.0f);
            float newTimeScale = MiMath::Lerp(m_targetTimeScale, m_startTimeScale, t);
            FPS_SetTimeScale(newTimeScale);

            if (t >= 1.0f) {
                FPS_SetTimeScale(m_startTimeScale); // 確実に元のタイムスケールに戻す
                m_isFinished = true; // タスク完了
            }
            break;
        }
    }
}


//------------------------------- public

// タイムスケール変更
void GameEffectController::ChangeTimeScale(float timeScale, float duration)
{
    if (m_changeTimeScaleTask) {
        m_changeTimeScaleTask->Cancel();
        delete m_changeTimeScaleTask;
    }

    m_changeTimeScaleTask = new ChangeTimeScaleTask(timeScale, duration);
    m_changeTimeScaleTask->Start();
}
void GameEffectController::ChangeTimeScaleTemporary(float timeScale, float duration, float holdDuration)
{
    if (m_changeTimeScaleTask) {
        m_changeTimeScaleTask->Cancel();
        delete m_changeTimeScaleTask;
    }

    m_changeTimeScaleTask = new ChangeTimeScaleTask(timeScale, duration, holdDuration);
    m_changeTimeScaleTask->Start();
}
// タイムスケールを元に戻す
void GameEffectController::ResetTimeScale(float duration)
{
    ChangeTimeScale(1.0f, duration);
}
