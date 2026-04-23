//---------------------------------------------------
// health_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/28
//---------------------------------------------------
#ifndef HEALTH_BEHAVIOR_H
#define HEALTH_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"

class HealthBehavior : public BehaviorComponent {
private:
    float m_health = 100.0f;
    float m_maxHealth = 100.0f;

public:
    HealthBehavior() {}
    ~HealthBehavior() {}

    void Start() override;
    void Update() override;

    void DrawComponentInspector() override;

public:
    virtual void TakeDamage(float damage);

};

#endif // HEALTH_BEHAVIOR_H