#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class PlayerMoveBehavior;
class PlayerAttackBehavior;

// プレイヤーの状態型
enum class PlayerState {
    Idle,
    Move,
    Attack,
    Dodge,
    Stunned,
};
enum class PlayerCombatState {

};
enum class PlayerVisualState {

};

// プレイヤー入力構造体
struct PlayerInput {
    float horizontal;   // 水平方向の入力
    float vertical;     // 垂直方向の入力
    XMFLOAT3 moveInput; // 移動入力（horizontal, verticalをカメラから変換）

    // ジャンプ入力
    bool    triggerJumpCommand;

    // 攻撃入力
    bool    triggerAttackCommand;
    bool    holdAttackCommand;
    bool    releaseAttackCommand;

    PlayerInput() : 
        horizontal(0.0f), 
        vertical(0.0f), 
        moveInput(0.0f, 0.0f, 0.0f),
        triggerJumpCommand(false), 
        triggerAttackCommand(false),
        holdAttackCommand(false), 
        releaseAttackCommand(false) {
    }
};

// プレイヤーコンテキスト構造体
struct PlayerContext {
    // プレイヤー入力
    PlayerInput input;

    // プレイヤーの状態
    PlayerState state;
    PlayerCombatState combatState;
    PlayerVisualState visualState;

    // StateMachineから利用するプレイヤー機能
    PlayerMoveBehavior* moveBehavior = nullptr;
    PlayerAttackBehavior* attackBehavior = nullptr;

    // CombatMachineから利用する武器


};