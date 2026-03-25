//---------------------------------------------------
// player_input_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//---------------------------------------------------
#ifndef PLAYER_INPUT_BEHAVIOR_H
#define PLAYER_INPUT_BEHAVIOR_H
#include "behavior_component.h"

class PlayerInputBehavior : public BehaviorComponent {
private:


public:
    PlayerInputBehavior() {}
    ~PlayerInputBehavior() {}

    void Start() override;
    void Update() override;

    void DrawComponentInspector() override;

};

#endif // PLAYER_INPUT_BEHAVIOR_H