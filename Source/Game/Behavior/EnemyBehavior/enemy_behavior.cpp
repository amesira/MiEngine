//===================================================
// enemy_behavior.cpp
// 
// Author・Miu Kitamura
// Date  ・・026/06/06
//===================================================
#include "enemy_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Utility/mi_math.h"

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

void EnemyBehavior::Start()
{
    GameObject* owner = GetOwner();
    if (!owner) return;

    m_context.enemyBehavior = this;
    m_context.transform = owner->GetComponent<TransformComponent>();
    m_context.rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_context.stateMachine = owner->GetComponent<EnemyStateMachineBehavior>();
    m_context.attackBehavior = owner->GetComponent<BaseEnemyAttackBehavior>();

    if (m_context.attackBehavior) {
        m_context.attackType = m_context.attackBehavior->GetAttackType();
    }

    IScene* scene = owner->GetScene();
    if (scene) {
        m_context.targetObject = scene->GetGameObjectByName("Player");
        if (m_context.targetObject) {
            m_context.targetTransform = m_context.targetObject->GetComponent<TransformComponent>();
        }
    }
}

void EnemyBehavior::Update()
{
    UpdateTargetInformation();

    const float deltaTime = FPS_GetDeltaTime();
    if (m_context.stateMachine) {
        m_context.stateMachine->UpdateStateMachine(m_context, deltaTime);
    }
}

void EnemyBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Enemy Behavior")) {
        ImGui::Text("State: %s", ToEnemyStateName(m_context.state));
        ImGui::Text("Target: %s", m_context.targetObject ? m_context.targetObject->GetName().c_str() : "None");
        ImGui::Text("Distance To Target: %.2f", m_context.distanceToTarget);
        ImGui::Text("Can See Target: %s", m_context.canSeeTarget ? "true" : "false");
    }

    InspectorViewWindow::EndComponentSection();
}

void EnemyBehavior::UpdateTargetInformation()
{
    m_context.distanceToTarget = 0.0f;
    m_context.canSeeTarget = false;

    if (!m_context.transform || !m_context.targetTransform) return;

    const DirectX::XMFLOAT3 enemyPosition = m_context.transform->GetPosition();
    const DirectX::XMFLOAT3 targetPosition = m_context.targetTransform->GetPosition();
    const DirectX::XMFLOAT3 toTarget = MiMath::Subtract(targetPosition, enemyPosition);

    m_context.distanceToTarget = MiMath::Length(toTarget);
    m_context.canSeeTarget = true;
}
