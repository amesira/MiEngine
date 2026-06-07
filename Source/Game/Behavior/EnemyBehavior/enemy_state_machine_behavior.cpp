//===================================================
// enemy_state_machine_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/06
//===================================================
#include "enemy_state_machine_behavior.h"

#include "base_enemy_attack_behavior.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

namespace {
    // デバッグ用：EnemyStateを文字列に変換
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

// ----------------------------------------------- public

// 敵の状態更新処理
void EnemyStateMachineBehavior::UpdateStateMachine(EnemyContext& context, float deltaTime)
{
    // 遷移後最初のフレームかどうかを判定
    const bool entered = m_isEnterState;
    m_isEnterState = false;
    m_debugEntered = entered;

    switch (context.state) {
    case EnemyState::Idle: // === Idle State ===
        // ターゲットが見える場合はChase状態に遷移
        if (context.canSeeTarget) {
            ChangeState(context, EnemyState::Chase);
        }
        break;

    case EnemyState::Chase: // === Chase State ===
        // ターゲットが見えなくなった場合はIdle状態に遷移
        if (!context.canSeeTarget) {
            ChangeState(context, EnemyState::Idle);
        }
        // ターゲットが攻撃範囲内で攻撃可能な場合はAttack状態に遷移
        else if (context.attackBehavior && context.attackBehavior->CanAttack(context)) {
            ChangeState(context, EnemyState::Attack);
        }
        break;

    case EnemyState::Attack: // === Attack State ===
        if (entered && context.attackBehavior) {
            context.attackBehavior->StartAttack(context);
        }

        if (context.attackBehavior) {
            context.attackBehavior->UpdateAttack(context, deltaTime);

            // 攻撃が終了したらChase状態に遷移
            if (context.attackBehavior->IsAttackFinished()) {
                ChangeState(context, EnemyState::Chase);
            }
        }
        break;

    case EnemyState::Stunned: // === Stunned State ===

        break;

    case EnemyState::Dead: // === Dead State ===

        break;

    default:
        break;
    }

    m_debugState = context.state;
}

// ----------------------------------------------- private

// 敵の状態変更処理
void EnemyStateMachineBehavior::ChangeState(EnemyContext& context, EnemyState newState)
{
    if (context.state == newState) return;

    context.state = newState;
    m_isEnterState = true;
}
