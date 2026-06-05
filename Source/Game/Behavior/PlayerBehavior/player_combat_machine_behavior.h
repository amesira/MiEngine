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
    bool    m_isEnterCombatState = true;
    bool    m_debugEntered = false;
    PlayerCombatState m_debugCombatState = PlayerCombatState::None;

public:
    PlayerCombatMachineBehavior() = default;
    ~PlayerCombatMachineBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // プレイヤーの戦闘状態更新処理
    void UpdateCombatMachine(PlayerContext& context, float deltaTime, float unscaledDeltaTime);

private:
    void ChangeCombatState(PlayerContext& context, PlayerCombatState newState);

};

#endif // PLAYER_COMBAT_MACHINE_BEHAVIOR_H
