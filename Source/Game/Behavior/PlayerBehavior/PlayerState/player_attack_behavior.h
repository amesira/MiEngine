//---------------------------------------------------
// player_attack_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//---------------------------------------------------
#ifndef PLAYER_ATTACK_BEHAVIOR_H
#define PLAYER_ATTACK_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "Game/Behavior/PlayerBehavior/player_context.h"

class PlayerAttackBehavior : public BehaviorComponent {
public:
    PlayerAttackBehavior() {}
    ~PlayerAttackBehavior() {}

    void Start() override;
    void Update() override;

    void DrawComponentInspector() override;

    void StartAttackHoldBuffer(PlayerContext& context);
    void UpdateAttackHoldBuffer(PlayerContext& context, float deltaTime);

    void StartAim(PlayerContext& context);
    void UpdateAim(PlayerContext& context, float deltaTime);

    void SingleAttack(PlayerContext& context);
    void ChargeAttack(PlayerContext& context);

};

#endif // PLAYER_ATTACK_BEHAVIOR_H
