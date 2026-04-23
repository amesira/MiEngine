#pragma once

class PlayerMoveBehavior;
class PlayerAttackBehavior;

// プレイヤー入力構造体
struct PlayerInput {
    float moveX = 0.0f;         // 水平方向の移動入力（-1.0f～1.0f）
    float moveZ = 0.0f;         // 前後方向の移動入力（-1.0f～1.0f）
    bool triggerJump = false;   // ジャンプ入力
};

// プレイヤーコンテキスト構造体
struct PlayerContext {
    // プレイヤー入力
    PlayerInput input;

    // StateMachineから利用するプレイヤー機能
    PlayerMoveBehavior* moveBehavior = nullptr;
    PlayerAttackBehavior* attackBehavior = nullptr;

    // CombatMachineから利用する武器


};