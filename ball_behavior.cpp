//===================================================
// ball_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/01/08
//===================================================
#include "ball_behavior.h"
#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"

#include "mi_math.h"
#include "mi_fps.h"

#include "transform_component.h"
#include "rigidbody_component.h"
#include "collider_component.h"

BallBehavior::BallBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<BallBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_collider = owner->GetComponent<SphereColliderComponent>();

    m_destroyTimer = -1.0f;
}

BallBehavior::~BallBehavior()
{

}

void BallBehavior::Update(IScene* pScene)
{
    float deltaTime = FPS_GetDeltaTime();

    // 自動破棄タイマー
    if(m_destroyTimer > 0.0f) {
        m_destroyTimer -= deltaTime;
        if(m_destroyTimer <= 0.0f) {
            GetOwner()->Destroy();
            return;
        }
    }

    // 設置確認
    if (m_collider->GetMaxMtv().y > 0.003f) {
        m_isGrounded = true;
    }
    else {
        m_isGrounded = false;
    }

    m_radius = m_collider->GetRadius();
    m_transform->SetScaling({ m_radius * 3.0f, m_radius * 3.0f, m_radius * 3.0f });

    // 速度設定
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();

    if(m_moveDirection.x != 0.0f || m_moveDirection.y != 0.0f || m_moveDirection.z != 0.0f) {
        m_moveDirection = MiMath::Normalize(m_moveDirection);
        velocity.x += m_moveDirection.x * 30.0f * deltaTime;
        velocity.z += m_moveDirection.z * 30.0f * deltaTime;
        velocity.x = std::clamp(velocity.x, -6.0f, 6.0f);
        velocity.z = std::clamp(velocity.z, -6.0f, 6.0f);
    }
    
    // 跳ね返り
    velocity.x += m_bounceVelocity.x;
    velocity.y += m_bounceVelocity.y;
    velocity.z += m_bounceVelocity.z;

    m_rigidbody->SetVelocity(velocity);
    m_moveDirection = { 0.0f,0.0f,0.0f };
    m_bounceVelocity = { 0.0f,0.0f,0.0f };

    // ボール回転更新
    UpdateBallRotation(deltaTime);

    // 物体破壊更新
    if(MiMath::Length(velocity) > 3.0f) {
        UpdateBreakObjects();
    }
}

// ボール回転の更新
void BallBehavior::UpdateBallRotation(float deltaTime)
{
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    velocity.y = 0.0f;

    // 回転量計算
    XMFLOAT3 prevDiff = {
        m_transform->GetPosition().x - m_transform->GetPrevPosition().x,
        0.0f,
        m_transform->GetPosition().z - m_transform->GetPrevPosition().z,
    };
    float speed = MiMath::Length(prevDiff) * 100.0f;

    // 回転適用
    if (speed > 0.1f && MiMath::Length(m_rigidbody->GetVelocity()) > 2.0f) {
        XMVECTOR quaternion = m_transform->GetRotation();

        // 回転軸
        XMVECTOR    vec1, vec2;
        vec1 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向
        vec2 = XMLoadFloat3(&velocity);             // 進行方向
        vec2 = XMVector3Normalize(vec2);
        XMVECTOR axis = XMVector3Cross(vec1, vec2); // 外積で回転軸を求める
        if (XMVectorGetX(XMVector3LengthSq(axis)) < 0.0001f) {
            axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // 進行方向が上方向と同じ場合はX軸を回転軸にする
        }

        // 回転量
        XMVECTOR    qu;
        qu = XMQuaternionRotationAxis(
            axis, XMConvertToRadians(speed));
        quaternion = XMQuaternionMultiply(quaternion, qu);

        m_transform->SetRotation(quaternion);
    }
}

void BallBehavior::UpdateBreakObjects()
{
    for(int i = 0; i < ColliderComponent::MAX_COLLISION_DATA; i++) {
        auto collisionData = m_collider->GetCollisionData(i);
        if(!collisionData.GetCollisionStay()) continue;
        GameObject* otherObj = collisionData.m_other->GetOwner();

    }
}

XMFLOAT3 BallBehavior::GetPosition() const
{
    if (m_transform) {
        return m_transform->GetPosition();
    }
    return { 0.0f,0.0f,0.0f };
}

void BallBehavior::AddRadius(float deltaRadius)
{
    m_radius = m_collider->GetRadius() + deltaRadius;
    if (m_radius > 2.0f)m_radius = 2.0f;

    m_collider->SetRadius(m_radius);
}
