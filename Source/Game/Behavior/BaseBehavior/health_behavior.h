//---------------------------------------------------
// health_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/28
//---------------------------------------------------
#ifndef HEALTH_BEHAVIOR_H
#define HEALTH_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"

#include <functional>

class HealthBehavior : public BehaviorComponent {
private:
    using DamageCallback = std::function<void(float currentHealth, float damage)>;

    float m_health = 100.0f;
    float m_maxHealth = 100.0f;
    DamageCallback m_onTakeDamage;

public:
    HealthBehavior() {}
    ~HealthBehavior() {}

    void Start() override;
    void Update() override;

    void DrawComponentInspector() override;

public:
    virtual void TakeDamage(float damage);
    void SetMaxHealth(float maxHealth, bool fillHealth = true);
    void SetHealth(float health);

    float GetHealth() const { return m_health; }
    float GetMaxHealth() const { return m_maxHealth; }
    float GetHealthRate() const;
    bool IsDead() const { return m_health <= 0.0f; }

    // ダメージコールバックの設定
    void SetOnTakeDamageCallback(DamageCallback callback) {
        m_onTakeDamage = callback;
    }

};

#endif // HEALTH_BEHAVIOR_H
