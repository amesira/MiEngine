//===================================================
// player_combat_machine_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/23
//===================================================
#include "player_combat_machine_behavior.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "./PlayerState/player_attack_behavior.h"

#include "Utility/debug_ostream.h"

namespace {
    // デバッグ用：PlayerCombatStateを文字列に変換
    const char* ToCombatStateName(PlayerCombatState state)
    {
        switch (state) {
        case PlayerCombatState::None: return "None";
        case PlayerCombatState::AimHoldBuffer: return "AimHoldBuffer";
        case PlayerCombatState::Aim: return "Aim";
        case PlayerCombatState::SingleAttack: return "SingleAttack";
        case PlayerCombatState::ChargeAttack: return "ChargeAttack";
        default: return "Unknown";
        }
    }
}

void PlayerCombatMachineBehavior::Start()
{

}

void PlayerCombatMachineBehavior::Update()
{

}

void PlayerCombatMachineBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Player Combat Machine")) {
        ImGui::Text("Combat State: %s", ToCombatStateName(m_debugCombatState));
        ImGui::Text("Entered This Frame: %s", m_debugEntered ? "true" : "false");
    }

    InspectorViewWindow::EndComponentSection();
}

// ----------------------------------------------- public

// プレイヤーの戦闘状態更新処理
void PlayerCombatMachineBehavior::UpdateCombatMachine(
    PlayerContext& context,
    PlayerMoveRequest& moveRequest,
    float deltaTime,
    float unscaledDeltaTime)
{
    // PlayerAttackBehaviorがないと戦闘状態を管理できないため、必須コンポーネントとして存在を確認
    if (!context.attackBehavior) {
        ChangeCombatState(context, PlayerCombatState::None);
        hal::dout << "Warning: PlayerCombatMachineBehavior could not find PlayerAttackBehavior. CombatState is set to None." << std::endl;
        return;
    }

    // 遷移後最初のフレームかどうか
    const bool entered = m_isEnterCombatState;
    m_isEnterCombatState = false;
    m_debugEntered = entered;

    switch (context.combatState) {
    case PlayerCombatState::None: // === 通常状態 ===
        if (context.input.triggerAimCommand) {
            ChangeCombatState(context, PlayerCombatState::AimHoldBuffer);
        }
        break;

    case PlayerCombatState::AimHoldBuffer: // === エイムに移行するまでの待機状態 ===
        if (entered) {
            context.attackBehavior->StartAimHoldBuffer(context);
        }

        context.attackBehavior->UpdateAimHoldBuffer(context, deltaTime, unscaledDeltaTime);

        // ホールドバッファ完了でエイム状態へ
        if (context.attackBehavior->IsFinishedAimHoldBuffer()) {
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        // エイムコマンドを離す、またはエイム・攻撃コマンドを両方ともホールドしていない場合は通常状態へ
        else if (context.input.releaseAimCommand || !context.input.holdAimCommand) {
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;

    case PlayerCombatState::Aim: // === エイム状態 ===
        // エイム中の移動リクエスト設定
        moveRequest.canMove = true;
        moveRequest.canRotate = true;
        moveRequest.speedMultiplier = 0.6f;
        moveRequest.rotationMode = PlayerRotationMode::AimForward;

        if (entered) {
            context.attackBehavior->StartAim(context);
        }

        context.attackBehavior->UpdateAim(context, deltaTime, unscaledDeltaTime);

        // チャージ攻撃
        if (context.input.triggerAttackCommand || context.input.holdAttackCommand) {
            ChangeCombatState(context, PlayerCombatState::AttackHoldBuffer);
        }
        // 通常状態へ戻る
        else if (context.input.releaseAimCommand || !context.input.holdAimCommand) {
            context.attackBehavior->EndAim(context);
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;

    case PlayerCombatState::AttackHoldBuffer: // === 攻撃に移行するまでの待機状態 ===
        // エイム中と同様の移動リクエスト設定
        moveRequest.canMove = true;
        moveRequest.canRotate = true;
        moveRequest.speedMultiplier = 0.6f;
        moveRequest.rotationMode = PlayerRotationMode::AimForward;

        if (entered) {
            context.attackBehavior->StartAttackHoldBuffer(context);
        }

        context.attackBehavior->UpdateAttackHoldBuffer(context, deltaTime, unscaledDeltaTime);

        if (context.attackBehavior->IsSingleAttackBuffer()) {
            // 攻撃コマンドを離したとき
            if (context.input.releaseAttackCommand || !context.input.holdAttackCommand) {
                // 単発攻撃へ
                ChangeCombatState(context, PlayerCombatState::SingleAttack);
            }
        }
        else {
            // チャージ攻撃へ
            ChangeCombatState(context, PlayerCombatState::ChargeAttack);
        }
        break;

    case PlayerCombatState::SingleAttack: // === 単発攻撃の状態 ===
        // 単発攻撃中の移動リクエスト設定
        moveRequest.canMove = false;
        moveRequest.canRotate = true;
        moveRequest.rotationMode = PlayerRotationMode::AimForward;

        // 単発攻撃の実行
        context.attackBehavior->SingleAttack(context);

        // エイム状態へ戻る
        ChangeCombatState(context, PlayerCombatState::Aim);
        break;

    case PlayerCombatState::ChargeAttack: // === チャージ攻撃の状態 ===
        // チャージ攻撃中の移動リクエスト設定
        moveRequest.canMove = true;
        moveRequest.canRotate = true;
        moveRequest.speedMultiplier = 0.4f;
        moveRequest.rotationMode = PlayerRotationMode::AimForward;

        if (entered) {
            context.attackBehavior->StartCharge(context);
        }

        context.attackBehavior->UpdateCharge(context, deltaTime, unscaledDeltaTime);

        if (context.input.releaseAttackCommand || !context.input.holdAttackCommand) {
            // チャージ攻撃の実行
            context.attackBehavior->ChargeAttack(context);
            // エイム状態へ戻る
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        break;

    default:
        ChangeCombatState(context, PlayerCombatState::None);
        break;
    }

    m_debugCombatState = context.combatState;
}

// ----------------------------------------------- private

// 戦闘状態の変更処理
void PlayerCombatMachineBehavior::ChangeCombatState(PlayerContext& context, PlayerCombatState newState)
{
    if (context.combatState == newState) return;

    context.combatState = newState;
    m_isEnterCombatState = true;
}
