//===================================================
// base_enemy_attack_behavior.cpp
// 
// Author・Miu Kitamura
// Date  ・・026/06/06
//===================================================
#include "base_enemy_attack_behavior.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

namespace {
const char* ToEnemyAttackTypeName(EnemyAttackType attackType)
{
    switch (attackType) {
    case EnemyAttackType::Melee: return "Melee";
    case EnemyAttackType::Shooter: return "Shooter";
    case EnemyAttackType::Area: return "Area";
    default: return "Unknown";
    }
}
}

void BaseEnemyAttackBehavior::Start()
{

}

void BaseEnemyAttackBehavior::Update()
{

}

void BaseEnemyAttackBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Enemy Attack")) {
        int attackType = static_cast<int>(m_attackType);
        const char* items[] = { "Melee", "Shooter", "Area" };
        if (ImGui::Combo("Attack Type", &attackType, items, 3)) {
            m_attackType = static_cast<EnemyAttackType>(attackType);
        }

        ImGui::DragFloat("Attack Range", &m_attackRange, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Attack Duration", &m_attackDuration, 0.01f, 0.0f, 10.0f);
        ImGui::Text("Current Type: %s", ToEnemyAttackTypeName(m_attackType));
        ImGui::Text("Attacking: %s", m_isAttacking ? "true" : "false");
        ImGui::Text("Attack Timer: %.2f", m_attackTimer);
    }

    InspectorViewWindow::EndComponentSection();
}

bool BaseEnemyAttackBehavior::CanAttack(const EnemyContext& context) const
{
    return context.canSeeTarget && context.distanceToTarget <= m_attackRange;
}

void BaseEnemyAttackBehavior::StartAttack(EnemyContext&)
{
    m_attackTimer = 0.0f;
    m_isAttacking = true;
}

void BaseEnemyAttackBehavior::UpdateAttack(EnemyContext&, float deltaTime)
{
    if (!m_isAttacking) return;

    m_attackTimer += deltaTime;
    if (m_attackTimer >= m_attackDuration) {
        m_isAttacking = false;
    }
}

bool BaseEnemyAttackBehavior::IsAttackFinished() const
{
    return !m_isAttacking;
}
