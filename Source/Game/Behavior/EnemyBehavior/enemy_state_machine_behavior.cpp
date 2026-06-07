//===================================================
// enemy_state_machine_behavior.cpp
// 
// Author・Miu Kitamura
// Date  ・・026/06/06
//===================================================
#include "enemy_state_machine_behavior.h"

#include "base_enemy_attack_behavior.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

namespace {
const char* ToEnemyStateName(EnemyState state)
{
    switch (state) {
    case EnemyState::Idle: return "Idle";
    case EnemyState::Chase: return "Chase";
    case EnemyState::Attack: return "Attack";
    case EnemyState::Stunned: return "Stunned";
    case EnemyState::Dead: return "Dead";
    default: return "Unknown";
    }
}
}

void EnemyStateMachineBehavior::Start()
{

}

void EnemyStateMachineBehavior::Update()
{

}

void EnemyStateMachineBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Enemy State Machine")) {
        ImGui::Text("State: %s", ToEnemyStateName(m_debugState));
        ImGui::Text("Entered This Frame: %s", m_debugEntered ? "true" : "false");
    }

    InspectorViewWindow::EndComponentSection();
}

void EnemyStateMachineBehavior::UpdateStateMachine(EnemyContext& context, float deltaTime)
{
    const bool entered = m_isEnterState;
    m_isEnterState = false;
    m_debugEntered = entered;

    switch (context.state) {
    case EnemyState::Idle:
        if (context.canSeeTarget) {
            ChangeState(context, EnemyState::Chase);
        }
        break;

    case EnemyState::Chase:
        if (context.attackBehavior && context.attackBehavior->CanAttack(context)) {
            ChangeState(context, EnemyState::Attack);
        }
        break;

    case EnemyState::Attack:
        if (entered && context.attackBehavior) {
            context.attackBehavior->StartAttack(context);
        }

        if (context.attackBehavior) {
            context.attackBehavior->UpdateAttack(context, deltaTime);
            if (context.attackBehavior->IsAttackFinished()) {
                ChangeState(context, EnemyState::Chase);
            }
        }
        else {
            ChangeState(context, EnemyState::Chase);
        }
        break;

    case EnemyState::Stunned:
    case EnemyState::Dead:
    default:
        break;
    }

    m_debugState = context.state;
}

void EnemyStateMachineBehavior::ChangeState(EnemyContext& context, EnemyState newState)
{
    if (context.state == newState) return;

    context.state = newState;
    m_isEnterState = true;
}
