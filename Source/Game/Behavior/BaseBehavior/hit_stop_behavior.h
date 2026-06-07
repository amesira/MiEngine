//---------------------------------------------------
// hit_stop_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/06
//---------------------------------------------------
#ifndef HIT_STOP_BEHAVIOR_H
#define HIT_STOP_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"
#include "Engine/Core/GamePlay/sequence_task.h"

#include <functional>

class HitStopBehavior : public BehaviorComponent {
public:
    // ヒットストップのコールバックの型定義
    using HitStopCallback = std::function<void()>;

private:
    // ヒットストップ処理を管理するタスククラス
    class HitStopTask : public SequenceTask {
    public:
        float m_duration = 0.0f;

        HitStopCallback m_onEnter = nullptr;
        HitStopCallback m_onEntered = nullptr;
        HitStopCallback m_onUpdate = nullptr;
        HitStopCallback m_onExit = nullptr;

        void Start() override;
        void Update(float deltaTime) override;
        bool IsRunning() const { return m_isRunning; }
    };

    HitStopTask m_hitStopTask;

public:
    HitStopBehavior() = default;
    ~HitStopBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // ヒットストップの開始
    void StartHitStop(
        float duration,
        HitStopCallback onEnter = nullptr,
        HitStopCallback onEntered = nullptr,
        HitStopCallback onUpdate = nullptr,
        HitStopCallback onExit = nullptr);

    // ヒットストップ中かどうかの判定
    bool IsHitStopping() const;
};

#endif // HIT_STOP_BEHAVIOR_H
