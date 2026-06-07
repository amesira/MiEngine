//===================================================
// health_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/28
//===================================================
#include "health_behavior.h"

#include "External/ImGui/imgui.h"

void HealthBehavior::Start()
{

}

void HealthBehavior::Update()
{

}

void HealthBehavior::DrawComponentInspector()
{
    ImGui::DragFloat("Health", &m_health, 1.0f, 0.0f, m_maxHealth);
    ImGui::DragFloat("Max Health", &m_maxHealth, 1.0f, 1.0f, 9999.0f);
    if (m_health > m_maxHealth) {
        m_health = m_maxHealth;
    }
}

void HealthBehavior::TakeDamage(float damage)
{
    m_health -= damage;
    if (m_health < 0.0f) {
        m_health = 0.0f;
    }
}

void HealthBehavior::SetMaxHealth(float maxHealth, bool fillHealth)
{
    m_maxHealth = maxHealth > 1.0f ? maxHealth : 1.0f;
    if (fillHealth) {
        m_health = m_maxHealth;
    }
    else if (m_health > m_maxHealth) {
        m_health = m_maxHealth;
    }
}

void HealthBehavior::SetHealth(float health)
{
    m_health = health;
    if (m_health < 0.0f) {
        m_health = 0.0f;
    }
    else if (m_health > m_maxHealth) {
        m_health = m_maxHealth;
    }
}

float HealthBehavior::GetHealthRate() const
{
    if (m_maxHealth <= 0.0f) return 0.0f;
    return m_health / m_maxHealth;
}
