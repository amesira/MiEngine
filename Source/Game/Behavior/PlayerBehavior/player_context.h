#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class PlayerMoveBehavior;
class PlayerAttackBehavior;
class PlayerDodgeBehavior;
class PlayerBehavior;

// プレイヤーの状態型
enum class PlayerState {
    Idle,
    Move,
    Dodge,
    Stunned,
};
enum class PlayerCombatState {
    None,
    AimHoldBuffer,
    Aim,

    AttackHoldBuffer,
    SingleAttack,
    ChargeAttack,
};

// プレイヤーエフェクトの種類
enum class PlayerEffectType {
    DodgeStart,
    DodgeEnd,

    AimHoldStart,

    AimStart,
    AimEnd,

    SingleAttack,
    SingleHit,

    ChargeStart,
    ChargeAttack,
    ChargeHit,

    AttackEnd,
};

// プレイヤーの回転モード
enum class PlayerRotationMode {
    CameraForward,
    AimForward,
    Locked,
};

// プレイヤーの移動リクエスト構造体
struct PlayerMoveRequest {
    bool canMove = true;
    bool canRotate = true;

    float speedMultiplier = 1.0f;

    PlayerRotationMode rotationMode = PlayerRotationMode::CameraForward;
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
    PlayerState state = PlayerState::Idle;
    PlayerCombatState combatState = PlayerCombatState::None;

    // StateMachineから利用するプレイヤー機能
    PlayerMoveBehavior* moveBehavior = nullptr;
    PlayerAttackBehavior* attackBehavior = nullptr;
    PlayerDodgeBehavior* dodgeBehavior = nullptr;
    PlayerBehavior* playerBehavior = nullptr;

    // CombatMachineから利用する機能


};
