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

class HealthBehavior;
class HitStopBehavior;

class SpriteRendererComponent;

class EnemyBehavior : public BehaviorComponent {
private:
    SpriteRendererComponent* m_spriteRenderer = nullptr;
    EnemyContext m_context;

    // カメラ参照
    CameraComponent* m_mainCamera = nullptr;

    float m_currentAngleY = 0.0f;

    HealthBehavior* m_healthBehavior = nullptr;
    HitStopBehavior* m_hitStopBehavior = nullptr;

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
