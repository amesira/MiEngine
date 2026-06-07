//===================================================
// bullet_behavior.cpp
// 
// Author・Miu Kitamura
// Date  ・・026/06/06
//===================================================
#include "bullet_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Processor/PhysicsPass/Collision/collision_query.h"

#include "External/ImGui/imgui.h"

#include <algorithm>
#include <cmath>

namespace {
    float Length(const DirectX::XMFLOAT3& value)
    {
        return std::sqrt(
            value.x * value.x +
            value.y * value.y +
            value.z * value.z);
    }

    DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
    {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z
        };
    }

    DirectX::XMFLOAT3 Subtract(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
    {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z
        };
    }

    DirectX::XMFLOAT3 Multiply(const DirectX::XMFLOAT3& value, float scalar)
    {
        return {
            value.x * scalar,
            value.y * scalar,
            value.z * scalar
        };
    }

    DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& value, float length)
    {
        if (length <= 0.0f) {
            return { 0.0f, 0.0f, 0.0f };
        }

        const float invLength = 1.0f / length;
        return Multiply(value, invLength);
    }
}

void BulletBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
    SetRadius(m_radius);
}

void BulletBehavior::Update()
{
    if (m_isExpired) return;

    if (!m_transform) {
        m_transform = GetOwner()->GetComponent<TransformComponent>();
    }
    if (!m_transform) {
        Expire();
        return;
    }

    const float deltaTime = FPS_GetDeltaTime();

    if (m_lifeTime >= 0.0f) {
        m_lifeTimer += deltaTime;
        if (m_lifeTimer >= m_lifeTime) {
            Expire();
            return;
        }
    }

    const DirectX::XMFLOAT3 previousPosition = m_transform->GetPosition();
    const DirectX::XMFLOAT3 nextPosition = Add(previousPosition, Multiply(m_velocity, deltaTime));
    const DirectX::XMFLOAT3 displacement = Subtract(nextPosition, previousPosition);
    const float moveDistance = Length(displacement);

    constexpr float minCastDistance = 0.0001f;
    if (moveDistance > minCastDistance && GetOwner()->GetScene()) {
        RaycastHit hit;
        const DirectX::XMFLOAT3 direction = Normalize(displacement, moveDistance);

        if (CollisionQuery::SphereCast(
            GetOwner()->GetScene(),
            /*out*/ hit,
            previousPosition,
            direction,
            m_radius,
            moveDistance,
            m_layerMask)) {
            m_lastHit = hit;
            m_hasHit = true;
            m_transform->SetPosition(hit.hitPoint);

            if (m_onHit) {
                m_onHit(m_lastHit);
            }

            Expire();
            return;
        }
    }

    m_transform->SetPosition(nextPosition);
}

void BulletBehavior::DrawComponentInspector()
{
    ImGui::DragFloat3("Velocity", &m_velocity.x, 0.1f);
    if (ImGui::DragFloat("Radius", &m_radius, 0.01f, 0.0f, 100.0f)) {
        SetRadius(m_radius);
    }
    ImGui::DragFloat("Life Time", &m_lifeTime, 0.01f, -1.0f, 100.0f);
    ImGui::InputInt("Layer Mask", &m_layerMask);
    ImGui::Text("Life Timer: %.3f", m_lifeTimer);
    ImGui::Text("Expired: %s", m_isExpired ? "true" : "false");
    ImGui::Text("Hit: %s", m_hasHit ? "true" : "false");
}

void BulletBehavior::Initialize(const DirectX::XMFLOAT3& velocity, float radius, float lifeTime, int layerMask)
{
    m_velocity = velocity;
    SetRadius(radius);
    m_lifeTime = lifeTime;
    m_layerMask = layerMask;
    m_lifeTimer = 0.0f;
    m_isExpired = false;
    m_hasHit = false;
    m_lastHit = {};
}

void BulletBehavior::SetRadius(float radius)
{
    m_radius = radius < 0.0f ? 0.0f : radius;

    if (!m_transform && GetOwner()) {
        m_transform = GetOwner()->GetComponent<TransformComponent>();
    }

    if (m_transform) {
        const float diameter = m_radius * 2.0f;
        m_transform->SetScaling({ diameter, diameter, diameter });
    }
}

void BulletBehavior::Expire()
{
    if (m_isExpired) return;

    m_isExpired = true;

    if (GetOwner()) {
        GetOwner()->Destroy();
    }
}
