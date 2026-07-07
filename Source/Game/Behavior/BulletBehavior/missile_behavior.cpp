//===================================================
// missile_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/07/07
//===================================================
#include "missile_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Device/mi_fps.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/model_component.h"

#include "Engine/Framework/Processor/PhysicsPass/Collision/collision_query.h"

#include "Game/Behavior/BaseBehavior/health_behavior.h"
#include "Game/Behavior/BaseBehavior/hit_stop_behavior.h"
#include "Game/Behavior/BaseBehavior/blinker_behavior.h"
#include "Game/Behavior/BaseBehavior/shake_object_behavior.h"

#include "External/ImGui/imgui.h"

#include "Game/Factory/render_effect_factory.h"

#include "Utility/mi_math.h"
using namespace DirectX;

MissileBehavior::MissileBehavior()
{
    m_timer = 0.0f;
    m_duration = 1.0f;
}

void MissileBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
    m_hitStopBehavior = GetOwner()->GetComponent<HitStopBehavior>();
    m_blinkerBehavior = GetOwner()->GetComponent<BlinkerBehavior>();
}

void MissileBehavior::Update()
{
    if (m_isExpired || !m_transform) return;

    const float deltaTime = FPS_GetDeltaTime();

    m_timer += deltaTime;
    if (m_timer > m_duration) {
        // TODO: 爆発とか
        Vanish();
        return;
    }

    // === 移動と衝突判定 ===
    float t = m_timer / m_duration;

    const XMFLOAT3 previousPosition = m_transform->GetPosition();
    const XMFLOAT3 nextPosition = EvaluateMissileTrajectory(m_controlPoint, 4, t);
    const XMFLOAT3 displacement = MiMath::Subtract(nextPosition, previousPosition);
    const float moveDistance = MiMath::Length(displacement);

    constexpr float minCastDistance = 0.0001f;
    if (moveDistance > minCastDistance && GetOwner()->GetScene()) {
        RaycastHit hit;
        const DirectX::XMFLOAT3 direction = MiMath::Normalize(displacement, moveDistance);

        // SphereCastで衝突判定を行う
        if (CollisionQuery::SphereCast(
            GetOwner()->GetScene(),
            /*out*/ hit,
            previousPosition,   // 開始位置
            direction,          // 方向
            m_radius,           // 半径
            moveDistance,       // 最大距離
            m_layerMask))
        {
            // ヒットした場合
            m_lastHit = hit;
            m_hasHit = true;

            bool isHitStop = false;
            if (hit.hitObject->GetName() == "Enemy") {
                XMFLOAT3 scale = m_transform->GetScaling();
                HealthBehavior* health = hit.hitObject->GetComponent<HealthBehavior>();
                health->TakeDamage(scale.x * 10.0f);

                if (health->IsDead()) {
                    isHitStop = true;
                }
            }

            // ヒットポイントに弾を移動させる
            m_transform->SetPosition(hit.hitPoint);

            // 消滅処理
            Vanish(isHitStop);
            return;
        }
    }

    // 衝突しなかった場合は通常通り移動
    m_transform->SetPosition(nextPosition);

    // 進行方向に回転
    XMFLOAT3 forward = MiMath::Multiply(MiMath::Normalize(displacement), -1.0f);
    XMFLOAT4 rotation = MiMath::QuaternionFromDirection(forward, { 0.0f, 1.0f, 0.0f });
    m_transform->SetRotation(rotation);
}

void MissileBehavior::DrawComponentInspector()
{

}

// ミサイル弾の初期化
void MissileBehavior::Initialize(
    const DirectX::XMFLOAT3& startPosition, 
    const DirectX::XMFLOAT3& controlPoint1, 
    const DirectX::XMFLOAT3& controlPoint2, 
    const DirectX::XMFLOAT3& targetPosition, 
    float duration,
    float radius,
    int layerMask)
{
    m_controlPoint[0] = startPosition;
    m_controlPoint[1] = controlPoint1;
    m_controlPoint[2] = controlPoint2;
    m_controlPoint[3] = targetPosition;

    m_timer = 0.0f;
    m_duration = duration <= 0.0001f ? 0.0001f : duration;
    m_radius = radius < 0.0f ? 0.0f : radius;

    m_layerMask = layerMask;

    if (m_transform) {
        m_transform->SetPosition(startPosition);
    }
}

// ミサイルの軌道計算（t（durationからの割合）時点）
DirectX::XMFLOAT3 MissileBehavior::EvaluateMissileTrajectory(const DirectX::XMFLOAT3* controlPoints, int pointCount, float t)
{
    t = MiMath::Clamp(t, 0.0f, 1.0f);

    int n = pointCount - 1;

    XMFLOAT3 traj = XMFLOAT3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < pointCount; i++) {
        // Biの計算
        float Bi = 1.0f;
        for (int j = 0; j < i; j++) {
            Bi *= t;
        }
        for (int j = 0; j < pointCount - i - 1; j++) {
            Bi *= (1.0f - t);
        }

        // 二項係数
        float coeff = 1.0f;
        for (int p = 1; p <= i; p++) {
            coeff *= static_cast<float>(n - p + 1) / static_cast<float>(p);
        }

        traj.x += coeff * Bi * controlPoints[i].x;
        traj.y += coeff * Bi * controlPoints[i].y;
        traj.z += coeff * Bi * controlPoints[i].z;
    }

    return traj;
}

// ミサイルの消滅処理
void MissileBehavior::Vanish(bool isHitStop)
{
    if (m_isExpired) return;
    m_isExpired = true;

    if (isHitStop) {
        // ヒットストップ処理
        m_hitStopBehavior->StartHitStop(
            0.5f,
            [this]() {
                m_blinkerBehavior->Flash({ 1.0f, 0.1f, 0.1f }, 1.0f, 0.5f);
            },
            nullptr,
            nullptr,
            [this]() {
                RenderEffectFactory::CreateExplosionEffect(GetOwner()->GetScene(), m_transform->GetPosition());
                if (GetOwner()) {
                    GetOwner()->Destroy();
                }
            });
    }
    else {
        RenderEffectFactory::CreateExplosionEffect(GetOwner()->GetScene(), m_transform->GetPosition());
        if (GetOwner()) {
            GetOwner()->Destroy();
        }
    }
}
