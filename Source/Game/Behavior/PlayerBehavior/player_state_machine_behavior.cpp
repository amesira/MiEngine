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
    switch (m_currentState) {

    case PlayerState::Idle: case PlayerState::Move: {
        // IdleとMoveの状態切り替え
        if (context.input.moveX != 0.0f || context.input.moveZ != 0.0f) {
            ChangeState(PlayerState::Move);
        }
        else {
            ChangeState(PlayerState::Idle);
        }

        // 移動と回転の更新
        context.moveBehavior->UpdateMove(context, deltaTime);
        context.moveBehavior->UpdateRotation(context, deltaTime);

        // 入力による状態切り替え


        break;
    }

    case PlayerState::Attack: {
        break;
    }

    case PlayerState::Dodge: {
        break;
    }

    default: break;
    }
}

// 状態切り替え
void PlayerStateMachineBehavior::ChangeState(PlayerState newState)
{
    if (m_currentState == newState) return;

    m_currentState = newState;
    m_isEnterState = true;
}
