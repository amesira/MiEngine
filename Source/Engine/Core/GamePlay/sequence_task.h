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
    int     m_taskStep = 0;
    float   m_taskTimer = 0.0f;

    bool    m_isFinished = false;

public:
    virtual ~SequenceTask() = default;
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;

    virtual bool IsFinished() const = 0;
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
        m_taskTimer += deltaTime;
        if (m_taskTimer >= seconds) {
            return true; // 待機完了
        }
        return false; // まだ待機中
    }
};

#endif // SEQUENCE_TASK_H