//===================================================
// player_combat_machine_behavior.cpp
// 
// Author・Miu Kitamura
// Date  ・・026/04/23
//===================================================
#include "player_combat_machine_behavior.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "./PlayerState/player_attack_behavior.h"

#include "Utility/debug_ostream.h"

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

void PlayerCombatMachineBehavior::UpdateCombatMachine(
    PlayerContext& context,
    PlayerMoveRequest& moveRequest,
    float deltaTime,
    float unscaledDeltaTime)
{
    if (!context.attackBehavior) {
        ChangeCombatState(context, PlayerCombatState::None);
        hal::dout << "Warning: PlayerCombatMachineBehavior could not find PlayerAttackBehavior. CombatState is set to None." << std::endl;
        return;
    }

    const bool entered = m_isEnterCombatState;
    m_isEnterCombatState = false;
    m_debugEntered = entered;

    switch (context.combatState) {
    case PlayerCombatState::None:
        if (context.input.triggerAimCommand) {
            ChangeCombatState(context, PlayerCombatState::HoldBuffer);
        }
        break;

    case PlayerCombatState::HoldBuffer:
        if (entered) {
            context.attackBehavior->StartAttackHoldBuffer(context);
        }

        context.attackBehavior->UpdateAttackHoldBuffer(context, deltaTime, unscaledDeltaTime);

        if (context.input.triggerAttackCommand || !context.input.holdAttackCommand) {
            ChangeCombatState(context, PlayerCombatState::SingleAttack);
        }
        else if (context.attackBehavior->IsFinishedHoldBuffer()) {
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        else if (context.input.releaseAimCommand || (!context.input.holdAimCommand && !context.input.holdAttackCommand)) {
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;

    case PlayerCombatState::Aim:
        moveRequest.canMove = true;
        moveRequest.canRotate = true;
        moveRequest.speedMultiplier *= 0.4f;
        moveRequest.rotationMode = PlayerRotationMode::AimForward;

        if (entered) {
            context.attackBehavior->StartAim(context);
        }

        context.attackBehavior->UpdateAim(context, deltaTime, unscaledDeltaTime);

        if (context.input.triggerAttackCommand || context.input.holdAttackCommand) {
            context.attackBehavior->EndAim(context);
            ChangeCombatState(context, PlayerCombatState::ChargeAttack);
        }
        else if (context.input.releaseAimCommand || (!context.input.holdAimCommand && !context.input.holdAttackCommand)) {
            context.attackBehavior->EndAim(context);
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;

    case PlayerCombatState::SingleAttack:
        moveRequest.canMove = false;
        moveRequest.canRotate = true;
        moveRequest.rotationMode = PlayerRotationMode::AimForward;

        if (entered) {
            context.attackBehavior->SingleAttack(context);
        }

        if (context.input.holdAimCommand) {
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        else {
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;

    case PlayerCombatState::ChargeAttack:
        moveRequest.canMove = false;
        moveRequest.canRotate = true;
        moveRequest.rotationMode = PlayerRotationMode::AimForward;

        if (entered) {
            context.attackBehavior->ChargeAttack(context);
        }

        if (context.input.holdAimCommand) {
            ChangeCombatState(context, PlayerCombatState::Aim);
        }
        else {
            ChangeCombatState(context, PlayerCombatState::None);
        }
        break;

    default:
        ChangeCombatState(context, PlayerCombatState::None);
        break;
    }

    m_debugCombatState = context.combatState;
}

void PlayerCombatMachineBehavior::ChangeCombatState(PlayerContext& context, PlayerCombatState newState)
{
    if (context.combatState == newState) return;

    context.combatState = newState;
    m_isEnterCombatState = true;
}
