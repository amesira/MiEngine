//---------------------------------------------------
// sequence_task.h
// 
// Author：Miu Kitamura
// Date  ：2026/05/25
//---------------------------------------------------
#ifndef SEQUENCE_TASK_H
#define SEQUENCE_TASK_H
#include "sequence_task.h"

class SequenceTask {
protected:
    bool    m_isRunning = false;

    int     m_taskStep = 0;
    float   m_taskTimer = 0.0f;

    bool    m_isFinished = false;

public:
    virtual ~SequenceTask() = default;
    virtual void Start() {
        m_taskStep = 0;
        m_taskTimer = 0.0f;
        m_isRunning = true;
        m_isFinished = false;
    }
    virtual void Update(float deltaTime) {
        m_taskTimer += deltaTime;
    }

    bool IsFinished() {
        return m_isFinished;
    }
    void Cancel() { 
        m_isFinished = true;
    }

    // タスクのステップを進める
    void AdvanceStep() {
        m_taskStep++;
        m_taskTimer = 0.0f;
    }

    // n秒間待機する
    bool Wait(float seconds, float deltaTime) {
        if (m_taskTimer >= seconds) {
            return true; // 待機完了
        }
        return false; // まだ待機中
    }
};

#endif // SEQUENCE_TASK_H