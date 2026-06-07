//---------------------------------------------------
// enemy_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/06
//---------------------------------------------------
#ifndef ENEMY_BEHAVIOR_H
#define ENEMY_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"
#include "enemy_context.h"

class CameraComponent;

class EnemyBehavior : public BehaviorComponent {
private:
    EnemyContext m_context;

    // カメラ参照
    CameraComponent* m_mainCamera = nullptr;

    float m_currentAngleY = 0.0f;

public:
    EnemyBehavior() = default;
    ~EnemyBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // コンテキストの取得
    EnemyContext& GetContext() { return m_context; }
    const EnemyContext& GetContext() const { return m_context; }

private:
    void UpdateTargetInformation();
};

#endif // ENEMY_BEHAVIOR_H
