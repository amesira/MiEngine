//===================================================
// bullet_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/06
//===================================================
#include "bullet_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Device/mi_fps.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/model_component.h"

#include "Engine/Framework/Processor/PhysicsPass/Collision/collision_query.h"

#include "health_behavior.h"
#include "hit_stop_behavior.h"
#include "blinker_behavior.h"
#include "shake_object_behavior.h"

#include "External/ImGui/imgui.h"

#include "Utility/mi_math.h"
using namespace DirectX;

void BulletBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
    if (!m_transform) {
        m_transform = GetOwner()->AddComponent<TransformComponent>();
    }
    m_hitStopBehavior = GetOwner()->GetComponent<HitStopBehavior>();
    if (!m_hitStopBehavior) {
        m_hitStopBehavior = GetOwner()->AddComponent<HitStopBehavior>();
    }
    m_blinkerBehavior = GetOwner()->GetComponent<BlinkerBehavior>();
    m_shakeObjectBehavior = GetOwner()->GetComponent<ShakeObjectBehavior>();

    // 半径に応じてスケーリングを設定
    SetRadius(m_radius);
}

void BulletBehavior::Update()
{
    if (m_isExpired) return; // すでに寿命切れの場合は処理しない

    const float deltaTime = FPS_GetDeltaTime();

    // ライフタイマーの更新と寿命切れの判定
    m_lifeTimer += deltaTime;
    if (m_lifeTimer >= m_lifeTime) {
        Finalize();
        return;
    }

    // === 移動と衝突判定 ===
    const XMFLOAT3 previousPosition = m_transform->GetPosition();
    const XMFLOAT3 nextPosition = MiMath::Add(previousPosition, MiMath::Multiply(m_velocity, deltaTime));
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

            // ヒットコールバックの呼び出し
            if (m_onHit) {
                m_onHit(m_lastHit);
            }

            // 終了処理
            Finalize(isHitStop);
            return;
        }
    }

    // 衝突しなかった場合は通常通り移動
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

// ----------------------------------------------- public

// 弾の初期化
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

// 弾の半径の設定
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

// 弾の終了処理
void BulletBehavior::Finalize(bool isHitStop)
{
    if (m_isExpired) return;
    m_isExpired = true;

    if (isHitStop) {
        m_hitStopBehavior->StartHitStop(
            0.1f,
            [this]() {
                m_blinkerBehavior->Flash({ 1.0f, 0.5f, 0.5f }, 1.0f, 0.1f);
                m_shakeObjectBehavior->Shake(0.1f, 0.5f);
            },
            nullptr,
            nullptr,
            [this]() {
                m_blinkerBehavior->Reset(0.1f);
                m_shakeObjectBehavior->Reset(0.1f);
                if (GetOwner()) {
                    GetOwner()->Destroy();
                }
            });
    }
    else {
        if (GetOwner()) {
            GetOwner()->Destroy();
        }
    }
}
