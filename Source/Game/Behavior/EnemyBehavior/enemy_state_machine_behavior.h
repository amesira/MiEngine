//---------------------------------------------------
// enemy_state_machine_behavior.h
// 
// Author・Miu Kitamura
// Date  ・・026/06/06
//---------------------------------------------------
#ifndef ENEMY_STATE_MACHINE_BEHAVIOR_H
#define ENEMY_STATE_MACHINE_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"
#include "enemy_context.h"

class EnemyStateMachineBehavior : public BehaviorComponent {
private:
    bool m_isEnterState = true;
    bool m_debugEntered = false;
    EnemyState m_debugState = EnemyState::Idle;

public:
    EnemyStateMachineBehavior() = default;
    ~EnemyStateMachineBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    void UpdateStateMachine(EnemyContext& context, float deltaTime);

private:
    void ChangeState(EnemyContext& context, EnemyState newState);
};

#endif // ENEMY_STATE_MACHINE_BEHAVIOR_H
