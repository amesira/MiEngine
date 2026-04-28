//---------------------------------------------------
// player_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef PLAYER_BEHAVIOR_H
#define PLAYER_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

#include "player_context.h"
class PlayerStateMachineBehavior;
class PlayerCombatMachineBehavior;
class PlayerVisualMachineBehavior;

class TransformComponent;
class CameraComponent;

class PlayerBehavior : public BehaviorComponent {
private:
    // プレイヤーコンテキスト
    PlayerContext m_context;

    // プレイヤー制御マシーン
    PlayerStateMachineBehavior* m_stateMachine = nullptr;
    PlayerCombatMachineBehavior* m_combatMachine = nullptr;
    PlayerVisualMachineBehavior* m_visualMachine = nullptr;

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

};

#endif