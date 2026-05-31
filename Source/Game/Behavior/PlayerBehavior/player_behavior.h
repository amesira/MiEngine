//---------------------------------------------------
// player_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef PLAYER_BEHAVIOR_H
#define PLAYER_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"

#include "Engine/Device/direct3d.h"
using namespace DirectX;

class SpriteRendererComponent;
class SpriteAnimationComponent;

#include "player_context.h"
class PlayerStateMachineBehavior;
class PlayerCombatMachineBehavior;

class TransformComponent;
class CameraComponent;

class PlayerBehavior : public BehaviorComponent {
private:
    SpriteRendererComponent* m_spriteRenderer = nullptr;
    SpriteAnimationComponent* m_spriteAnimation = nullptr;

    // プレイヤーコンテキスト
    PlayerContext m_context;

    // プレイヤー制御マシーン
    PlayerStateMachineBehavior* m_stateMachine = nullptr;
    PlayerCombatMachineBehavior* m_combatMachine = nullptr;

    // メインカメラの参照
    TransformComponent* m_mainCameraTransform = nullptr;
    CameraComponent* m_mainCamera = nullptr;

public:
    ~PlayerBehavior() = default;
    void    Start() override;
    void    Update() override;
    void    DrawComponentInspector() override;

private:
    // プレイヤーの入力処理
    PlayerInput UpdateInput();
    // プレイヤーのアニメーション制御
    void UpdateAnimation(PlayerState state, PlayerCombatState combatState);

};

#endif