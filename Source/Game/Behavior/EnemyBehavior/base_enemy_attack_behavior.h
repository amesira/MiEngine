//---------------------------------------------------
// base_enemy_attack_behavior.h
// 
// Author・Miu Kitamura
// Date  ・・026/06/06
//---------------------------------------------------
#ifndef BASE_ENEMY_ATTACK_BEHAVIOR_H
#define BASE_ENEMY_ATTACK_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"
#include "enemy_context.h"

class BaseEnemyAttackBehavior : public BehaviorComponent {
private:
    EnemyAttackType m_attackType = EnemyAttackType::Melee;
    float m_attackRange = 3.0f;
    float m_attackDuration = 0.5f;
    float m_attackTimer = 0.0f;
    bool m_isAttacking = false;

public:
    BaseEnemyAttackBehavior() = default;
    ~BaseEnemyAttackBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    EnemyAttackType GetAttackType() const { return m_attackType; }
    void SetAttackType(EnemyAttackType attackType) { m_attackType = attackType; }

    bool CanAttack(const EnemyContext& context) const;
    void StartAttack(EnemyContext& context);
    void UpdateAttack(EnemyContext& context, float deltaTime);
    bool IsAttackFinished() const;
};

#endif // BASE_ENEMY_ATTACK_BEHAVIOR_H
