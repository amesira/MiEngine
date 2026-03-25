//---------------------------------------------------
// player_attack_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//---------------------------------------------------
#ifndef PLAYER_ATTACK_BEHAVIOR_H
#define PLAYER_ATTACK_BEHAVIOR_H
#include "behavior_component.h"

class PlayerAttackBehavior : public BehaviorComponent {
private:

public:
    PlayerAttackBehavior() {}
    ~PlayerAttackBehavior() {}

    void Start() override;
    void Update() override;

    void DrawComponentInspector() override;

};

#endif // PLAYER_ATTACK_BEHAVIOR_H