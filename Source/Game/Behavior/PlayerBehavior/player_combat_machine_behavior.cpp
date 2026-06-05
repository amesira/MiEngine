//===================================================
// player_combat_machine_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/23
//===================================================
#include "player_combat_machine_behavior.h"

#include <windows.h>

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "./PlayerState/player_attack_behavior.h"

namespace {
const char* ToCombatStateName(PlayerCombatState state)
{
    switch (state) {
    case PlayerCombatState::None: return "None";
    case PlayerCombatState::HoldBuffer: return "HoldBuffer";
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

//------------------------------- public

// プレイヤーの戦闘状態更新処理
void PlayerCombatMachineBehavior::UpdateCombatMachine(PlayerContext& context, float deltaTime)
{
    bool entered = m_isEnterCombatState;
    m_isEnterCombatState = false;
    m_debugEntered = entered;

    switch (context.combatState) {
    case PlayerCombatState::None: {
        if (context.input.triggerAttackCommand) {
            ChangeCombatState(context, PlayerCombatState::HoldBuffer);
        }
        else if (context.input.holdAimCommand || context.input.triggerAimCommand) {
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        break;
    }

    case PlayerCombatState::HoldBuffer: {
        if (context.attackBehavior) {
            if (entered) {
                context.attackBehavior->StartAttackHoldBuffer(context);
            }

            context.attackBehavior->UpdateAttackHoldBuffer(context, deltaTime);
        }

        if (context.input.releaseAttackCommand || !context.input.holdAttackCommand) {
            ChangeCombatState(context, PlayerCombatState::SingleAttack);
        }
        else {
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        break;
    }

    case PlayerCombatState::Aim: {
        if (context.attackBehavior) {
            if (entered) {
                context.attackBehavior->StartAim(context);
            }

            context.attackBehavior->UpdateAim(context, deltaTime);
        }

        if (context.input.releaseAttackCommand) {
            ChangeCombatState(context, PlayerCombatState::ChargeAttack);
        }
        else if (context.input.releaseAimCommand || (!context.input.holdAimCommand && !context.input.holdAttackCommand)) {
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;
    }

    case PlayerCombatState::SingleAttack: {
        if (entered && context.attackBehavior) {
            context.attackBehavior->SingleAttack(context);
        }

        if (context.input.holdAimCommand) {
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        else {
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;
    }

    case PlayerCombatState::ChargeAttack: {
        if (entered && context.attackBehavior) {
            context.attackBehavior->ChargeAttack(context);
        }

        if (context.input.holdAimCommand) {
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        else {
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;
    }

    default:
        ChangeCombatState(context, PlayerCombatState::None);
        break;
    }

    m_debugCombatState = context.combatState;
}

// CombatStateの変更処理
void PlayerCombatMachineBehavior::ChangeCombatState(PlayerContext& context, PlayerCombatState newState)
{
    if (context.combatState == newState) return;

    context.combatState = newState;
    m_isEnterCombatState = true;
}
