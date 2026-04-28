//===================================================
// player_state_machine_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/23
//===================================================
#include "player_state_machine_behavior.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Utility/mi_math.h"
#include "Engine/System/Device/mi_fps.h"

#include "Engine/Editor/imgui_window_interface.h"

#include "./PlayerState/player_move_behavior.h"
#include "./PlayerState/player_attack_behavior.h"
#include "./PlayerState/player_dodge_behavior.h"

void PlayerStateMachineBehavior::Start()
{

}

void PlayerStateMachineBehavior::Update()
{

}

void PlayerStateMachineBehavior::DrawComponentInspector()
{

}

//------------------------------- private

// プレイヤーの状態更新処理
void PlayerStateMachineBehavior::UpdateStateMachine(PlayerContext& context, float deltaTime)
{
    // 状態に入ったばかりかどうかのフラグを取得してリセット
    bool entered = m_isEnterState;
    m_isEnterState = false;

    // 状態ごとの処理
    switch (context.state) {
    case PlayerState::Idle: case PlayerState::Move: {
        // IdleとMoveの状態切り替え
        if (MiMath::Length(context.input.moveInputCameraLocal) > 0.01f) {
            ChangeState(context, PlayerState::Move);
        }
        else {
            ChangeState(context, PlayerState::Idle);
        }

        // 移動と回転の更新
        context.moveBehavior->UpdateMove(context, deltaTime);
        context.moveBehavior->UpdateRotation(context, deltaTime);

        // 入力による状態切り替え
        if (context.input.triggerDashCommand) {
            ChangeState(context, PlayerState::Dodge);
        }
        else if (context.input.triggerAimCommand) {
            ChangeState(context, PlayerState::Attack);
        }

        break;
    }

    case PlayerState::Attack: {

        // 攻撃終了条件
        if (context.input.releaseAimCommand) {
            ChangeState(context, PlayerState::Idle);
        }

        break;
    }

    case PlayerState::Dodge: {
        // 回避開始処理
        if (entered) {
            context.dodgeBehavior->StartDodge(context);
        }

        // 移動と回転の更新（回避中は移動速度を上げる）
        context.moveBehavior->UpdateMove(context, deltaTime, 1.5f);
        context.moveBehavior->UpdateRotation(context, deltaTime);

        // 回避の更新
        context.dodgeBehavior->UpdateDodge(context, deltaTime);

        // 回避終了条件
        if (context.dodgeBehavior->IsDodgeFinished()) {
            ChangeState(context, PlayerState::Idle);
        }

        break;
    }

    default: break;
    }
}

// 状態切り替え
void PlayerStateMachineBehavior::ChangeState(PlayerContext& context, PlayerState newState)
{
    if (context.state == newState) return;

    context.state = newState;
    m_isEnterState = true;
}
