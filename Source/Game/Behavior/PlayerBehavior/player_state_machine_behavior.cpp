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
#include "Engine/Device/mi_fps.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "./PlayerState/player_move_behavior.h"
#include "./PlayerState/player_attack_behavior.h"
#include "./PlayerState/player_dodge_behavior.h"

namespace {
    const char* ToStateName(PlayerState state)
    {
        switch (state) {
        case PlayerState::Idle: return "Idle";
        case PlayerState::Move: return "Move";
        case PlayerState::Dodge: return "Dodge";
        case PlayerState::Stunned: return "Stunned";
        default: return "Unknown";
        }
    }
}

void PlayerStateMachineBehavior::Start()
{

}

void PlayerStateMachineBehavior::Update()
{

}

void PlayerStateMachineBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Player State Machine")) {
        ImGui::Text("Combat State: %s", ToStateName(m_debugState));
        ImGui::Text("Entered This Frame: %s", m_debugEntered ? "true" : "false");
    }

    InspectorViewWindow::EndComponentSection();
}

//------------------------------- private

// プレイヤーの状態更新処理
void PlayerStateMachineBehavior::UpdateStateMachine(PlayerContext& context, PlayerMoveRequest& moveRequest, float deltaTime)
{
    // 状態に入ったばかりかどうかのフラグを取得してリセット
    bool entered = m_isEnterState;
    m_isEnterState = false;
    m_debugEntered = entered;

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
        moveRequest.canMove = true;
        moveRequest.canRotate = true;
        moveRequest.speedMultiplier = 1.0f;
        moveRequest.rotationMode = PlayerRotationMode::CameraForward;

        // 入力による状態切り替え
        if (context.input.triggerDashCommand) {
            ChangeState(context, PlayerState::Dodge);
        }

        break;
    }
    case PlayerState::Dodge: {
        // 回避開始処理
        if (entered) {
            context.dodgeBehavior->StartDodge(context);
        }

        // 移動と回転の更新（回避中は移動速度を上げる）
        moveRequest.canMove = true;
        moveRequest.canRotate = true;
        moveRequest.speedMultiplier = 1.5f;
        moveRequest.rotationMode = PlayerRotationMode::CameraForward;

        // 回避の更新
        context.dodgeBehavior->UpdateDodge(context, deltaTime);

        // 回避終了条件
        if (context.dodgeBehavior->IsDodgeFinished()) {
            ChangeState(context, PlayerState::Idle);
        }

        break;
    }

    case PlayerState::Stunned: {
        moveRequest.canMove = false;
        moveRequest.canRotate = false;
        moveRequest.rotationMode = PlayerRotationMode::Locked;
        break;
    }

    default: break;
    }

    m_debugState = context.state;
}

// 状態切り替え
void PlayerStateMachineBehavior::ChangeState(PlayerContext& context, PlayerState newState)
{
    if (context.state == newState) return;

    context.state = newState;
    m_isEnterState = true;
}
