//---------------------------------------------------
// enemy_behavior.h
// 
// Author・Miu Kitamura
// Date  ・・026/06/06
//---------------------------------------------------
#ifndef ENEMY_BEHAVIOR_H
#define ENEMY_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"
#include "enemy_context.h"

class EnemyBehavior : public BehaviorComponent {
private:
    EnemyContext m_context;

public:
    EnemyBehavior() = default;
    ~EnemyBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    EnemyContext& GetContext() { return m_context; }
    const EnemyContext& GetContext() const { return m_context; }

private:
    void UpdateTargetInformation();
};

#endif // ENEMY_BEHAVIOR_H
