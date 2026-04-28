#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#include "player_visual_request.h"

class PlayerMoveBehavior;
class PlayerAttackBehavior;
class PlayerDodgeBehavior;

// プレイヤーの状態型
enum class PlayerState {
    Idle,
    Move,
    Attack,
    Dodge,
    Stunned,
};

// プレイヤー入力構造体
struct PlayerInput {
    float horizontal;   // 水平方向の入力
    float vertical;     // 垂直方向の入力
    XMFLOAT3 moveInputCameraLocal; // 移動入力（horizontal, verticalをカメラから変換）

    // ジャンプ入力
    bool    triggerJumpCommand;

    // ダッシュ入力
    bool    triggerDashCommand;

    // エイム入力
    bool    triggerAimCommand;
    bool    holdAimCommand;
    bool    releaseAimCommand;

    // 攻撃入力
    bool    triggerAttackCommand;
    bool    holdAttackCommand;
    bool    releaseAttackCommand;

    PlayerInput() : 
        horizontal(0.0f), 
        vertical(0.0f), 
        moveInputCameraLocal(0.0f, 0.0f, 0.0f),
        triggerJumpCommand(false),
        triggerDashCommand(false),
        triggerAimCommand(false),
        holdAimCommand(false),
        releaseAimCommand(false),
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

    // StateMachineから利用するプレイヤー機能
    PlayerMoveBehavior* moveBehavior = nullptr;
    PlayerAttackBehavior* attackBehavior = nullptr;
    PlayerDodgeBehavior* dodgeBehavior = nullptr;

    // CombatMachineから利用する武器

    // VisualMachineから利用する見た目関連のリクエスト
    PlayerVisualRequest visualRequest;

};