//---------------------------------------------------
// tween_task.h
// ・普遍的に利用できる補間タスクを定義するヘッダ
// 
// Author：Miu Kitamura
// Date  ：2026/06/01
//---------------------------------------------------
#ifndef TWEEN_TASK_H
#define TWEEN_TASK_H
#include "sequence_task.h"

#include <DirectXMath.h>
using namespace DirectX;

#include "Utility/mi_math.h"
#include <algorithm>

// float値を補間する演出タスク
class FloatTweenTask : public SequenceTask {
public:
    float m_startValue = 0.0f;  // 補間の開始値
    float m_targetValue = 0.0f; // 補間の目標値
    float m_endValue = 0.0f;    // 補間の終了値（目標値からさらに変化させる場合に利用）

    float m_currentValue = 0.0f;// 現在の補間値

    float m_duration = 0.0f;    // 補間にかける時間
    float m_holdDuration = 0.0f;// 目標値でどれだけの時間値を保持するか

    void Start() override {
        SequenceTask::Start();
        m_currentValue = m_startValue;
    }
    void Update(float deltaTime) override {
        if (IsFinished()) return;
        SequenceTask::Update(deltaTime);

        switch (m_taskStep) {
        case 0: {
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }
            m_currentValue = MiMath::Lerp(m_startValue, m_targetValue, t);

            if (t >= 1.0f) {
                AdvanceStep();
            }
            break;
        }
        case 1: {
            if (m_holdDuration > 0.0f) {
                if (Wait(m_holdDuration)) {
                    AdvanceStep();
                }
            }
            else {
                Finish();
            }
            break;
        }
        case 2: {
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }
            m_currentValue = MiMath::Lerp(m_targetValue, m_endValue, t);

            if (t >= 1.0f) {
                m_currentValue = m_endValue;
                Finish();
            }
            break;
        }
        }
    }
};

// XMFLOAT3値を補間する演出タスク
class Vector3TweenTask : public SequenceTask {
public:
    XMFLOAT3 m_startValue = { 0.0f, 0.0f, 0.0f };
    XMFLOAT3 m_targetValue = { 0.0f, 0.0f, 0.0f };
    XMFLOAT3 m_endValue = { 0.0f, 0.0f, 0.0f };
    XMFLOAT3 m_currentValue = { 0.0f, 0.0f, 0.0f };
    float m_duration = 0.0f;
    float m_holdDuration = 0.0f;

    void Start() override {
        SequenceTask::Start();
        m_currentValue = m_startValue;
    }
    void Update(float deltaTime) override {
        if (IsFinished()) return;
        SequenceTask::Update(deltaTime);

        switch (m_taskStep) {
        case 0: {
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }
            m_currentValue = MiMath::Lerp(m_startValue, m_targetValue, t);

            if (t >= 1.0f) {
                AdvanceStep();
            }
            break;
        }
        case 1: {
            if (m_holdDuration > 0.0f) {
                if (Wait(m_holdDuration)) {
                    AdvanceStep();
                }
            }
            else {
                Finish();
            }
            break;
        }
        case 2: {
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }
            m_currentValue = MiMath::Lerp(m_targetValue, m_endValue, t);

            if (t >= 1.0f) {
                m_currentValue = m_endValue;
                Finish();
            }
            break;
        }
        }
    }
};

#endif // TWEEN_TASK_H