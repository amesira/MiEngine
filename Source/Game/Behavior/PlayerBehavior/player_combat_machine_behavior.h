//---------------------------------------------------
// player_combat_machine_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/23
//---------------------------------------------------
#ifndef PLAYER_COMBAT_MACHINE_BEHAVIOR_H
#define PLAYER_COMBAT_MACHINE_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "player_context.h"

class PlayerCombatMachineBehavior : public BehaviorComponent {
private:


public:
    PlayerCombatMachineBehavior() = default;
    ~PlayerCombatMachineBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // プレイヤーの戦闘状態更新処理
    void UpdateCombatMachine(PlayerContext& context, float deltaTime);

private:

};

#endif // PLAYER_COMBAT_MACHINE_BEHAVIOR_H