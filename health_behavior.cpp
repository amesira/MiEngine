//===================================================
// health_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/28
//===================================================
#include "health_behavior.h"

void HealthBehavior::Start()
{

}

void HealthBehavior::Update()
{

}

void HealthBehavior::DrawComponentInspector()
{

}

void HealthBehavior::TakeDamage(float damage)
{
    m_health -= damage;
    if (m_health < 0.0f) {
        m_health = 0.0f;
    }
}
