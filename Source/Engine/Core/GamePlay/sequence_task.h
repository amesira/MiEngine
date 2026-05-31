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

public:
    virtual ~SequenceTask() = default;
    // タスクの開始（オーバーライドした際も、必ず基本のStart処理を呼び出すこと）
    virtual void Start() {
        m_taskStep = 0;
        m_taskTimer = 0.0f;
        m_isRunning = true;
    }
    // タスクの更新（オーバーライドした際も、必ず基本のUpdate処理を呼び出すこと）
    virtual void Update(float deltaTime) {
        m_taskTimer += deltaTime;
    }

    // タスクのリセット
    void Reset() {
        m_taskStep = 0;
        m_taskTimer = 0.0f;
        m_isRunning = false;
    }
    // タスクの終了
    void Finish() {
        m_isRunning = false;
    }

    // タスクの状態確認
    bool IsFinished() {
        return !m_isRunning;
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