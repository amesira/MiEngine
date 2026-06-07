//===================================================
// enemy_state_machine_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/06
//===================================================
#include "enemy_state_machine_behavior.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"

#include "base_enemy_attack_behavior.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/transform_component.h"
#include "Utility/mi_math.h"

#include "Game/Behavior/BaseBehavior/health_behavior.h"
#include "Game/Behavior/BaseBehavior/hit_stop_behavior.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

namespace {
    constexpr float ENEMY_CHASE_SPEED = 4.0f;

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

    void StopHorizontalVelocity(EnemyContext& context)
    {
        if (!context.rigidbody) return;

        XMFLOAT3 velocity = context.rigidbody->GetVelocity();
        velocity.x = 0.0f;
        velocity.z = 0.0f;
        context.rigidbody->SetVelocity(velocity);
    }

    void ChaseTarget(EnemyContext& context)
    {
        if (!context.transform || !context.targetTransform || !context.rigidbody) return;

        const XMFLOAT3 enemyPosition = context.transform->GetPosition();
        const XMFLOAT3 targetPosition = context.targetTransform->GetPosition();
        XMFLOAT3 toTarget = MiMath::Subtract(targetPosition, enemyPosition);
        toTarget.y = 0.0f;

        const float distance = MiMath::Length(toTarget);
        const XMFLOAT3 direction = MiMath::Normalize(toTarget, distance);

        XMFLOAT3 velocity = context.rigidbody->GetVelocity();
        velocity.x = direction.x * ENEMY_CHASE_SPEED;
        velocity.z = direction.z * ENEMY_CHASE_SPEED;
        context.rigidbody->SetVelocity(velocity);
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
        StopHorizontalVelocity(context);

        // ターゲットが見える場合はChase状態に遷移
        if (context.canSeeTarget) {
            ChangeState(context, EnemyState::Chase);
        }
        break;

    case EnemyState::Chase: // === Chase State ===
        ChaseTarget(context);

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
        StopHorizontalVelocity(context);

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
        StopHorizontalVelocity(context);

        break;

    case EnemyState::Dead: // === Dead State ===
        StopHorizontalVelocity(context);

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
