//---------------------------------------------------
// player_move_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//---------------------------------------------------
#ifndef PLAYER_MOVE_BEHAVIOR_H
#define PLAYER_MOVE_BEHAVIOR_H
#include "behavior_component.h"

class PlayerMoveBehavior : public BehaviorComponent {
private:

public:
    PlayerMoveBehavior() {}
    ~PlayerMoveBehavior() {}

    void Start() override;
    void Update() override;

    void DrawComponentInspector() override;

};

#endif // PLAYER_MOVE_BEHAVIOR_H