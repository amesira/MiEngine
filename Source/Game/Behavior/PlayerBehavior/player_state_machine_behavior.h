//---------------------------------------------------
// player_state_machine_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/23
//---------------------------------------------------
#ifndef PLAYER_STATE_MACHINE_BEHAVIOR_H
#define PLAYER_STATE_MACHINE_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "player_context.h"

class PlayerStateMachineBehavior : public BehaviorComponent {
private:
    // プレイヤーの状態型
    enum class PlayerState {
        Idle,
        Move,
        Attack,
        Dodge,
        Stunned,
    };

    PlayerState m_currentState = PlayerState::Idle; // 現在の状態
    bool    m_isEnterState = true;                  // 状態に入ったばかりかどうかのフラグ

public:
    PlayerStateMachineBehavior() = default;
    ~PlayerStateMachineBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // プレイヤーの状態更新処理
    void UpdateStateMachine(PlayerContext& context, float deltaTime);

private:
    // 状態切り替え
    void ChangeState(PlayerState newState);

};

#endif // PLAYER_STATE_MACHINE_BEHAVIOR_H