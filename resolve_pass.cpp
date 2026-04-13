//===================================================
// resolve_pass.cpp [物理演算補正プロセッサー]
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "resolve_pass.h"

#include "game_object.h"
#include "scene_interface.h"

#include "mi_math.h"

#include "transform_component.h"
#include "collider_component.h"
#include "rigidbody_component.h"

#include "mi_fps.h"

#include "debug_ostream.h"

// 接地判定の閾値
#define ON_GROUND_THRESHOLD (0.03f)

// 速度変化の最大値
#define REV_VELOCITY_MAX (400.0f)

void ResolvePass::Initialize()
{

}

void ResolvePass::Finalize()
{

}

void ResolvePass::Process(IScene* pScene)
{
    float deltaTime = FPS_GetDeltaTime();

    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* rigidbodyPool = pScene->GetComponentPool<RigidbodyComponent>();
    auto* boxColliderPool = pScene->GetComponentPool<BoxColliderComponent>();
    auto* sphereColliderPool = pScene->GetComponentPool<SphereColliderComponent>();
    if (transformPool == nullptr || rigidbodyPool == nullptr)return;

    if(boxColliderPool){
        auto& boxColliderList = boxColliderPool->GetList();

        for (BoxColliderComponent& c : boxColliderList) {
            BoxColliderComponent* collider = &c;
            RigidbodyComponent* rigidbody = rigidbodyPool->GetByGameObjectID(collider->GetOwner()->GetID());
            TransformComponent* transform = transformPool->GetByGameObjectID(collider->GetOwner()->GetID());

            // コンポーネントが無効ならスキップ
            if (!transform || !collider || !rigidbody)continue;
            if (!transform->GetEnable() || !collider->GetEnable() || !rigidbody->GetEnable())continue;

            if (rigidbody->GetIsKinematic()) {
                rigidbody->SetVelocity({ 0.0f, 0.0f, 0.0f });
                continue;
            }

            // 物理演算補正適用
            ApplyResolve(transform, collider, rigidbody, deltaTime);
        }
    }

    if(sphereColliderPool){
        auto& sphereColliderList = sphereColliderPool->GetList();

        for(SphereColliderComponent& c : sphereColliderList) {
            SphereColliderComponent* collider = &c;
            RigidbodyComponent* rigidbody = rigidbodyPool->GetByGameObjectID(collider->GetOwner()->GetID());
            TransformComponent* transform = transformPool->GetByGameObjectID(collider->GetOwner()->GetID());

            // コンポーネントが無効ならスキップ
            if (!transform || !collider || !rigidbody)continue;
            if (!transform->GetEnable() || !collider->GetEnable() || !rigidbody->GetEnable())continue;

            if (rigidbody->GetIsKinematic()) {
                rigidbody->SetVelocity({ 0.0f, 0.0f, 0.0f });
                continue;
            }

            // 物理演算補正適用
            ApplyResolve(transform, collider, rigidbody, deltaTime);
        }
    }
}

void ResolvePass::ApplyResolve(
    TransformComponent* transform, ColliderComponent* collider, RigidbodyComponent* rigidbody, float deltaTime)
{
    bool isGrounded = false;

    // 外力を算出
    XMFLOAT3 revVelocity = { 0.0f,0.0f,0.0f };
    {
        XMFLOAT3 min = { 0.0f, 0.0f, 0.0f };
        XMFLOAT3 max = { 0.0f, 0.0f, 0.0f };
        float minMtvY = 0.0f;

        // 衝突情報から最小移動ベクトルの最大値と最小値を算出
        auto collisionDataList = ColliderComponent::Internal::GetCollisionData(collider);
        for (const auto& data : collisionDataList) {
            if (data.m_other == nullptr || !data.m_isCollision) {
                continue;
            }
            
            if (data.m_correction.x < min.x)min.x = data.m_correction.x;
            if (data.m_correction.y < min.y)min.y = data.m_correction.y;
            if (data.m_correction.z < min.z)min.z = data.m_correction.z;

            if (data.m_correction.x > max.x)max.x = data.m_correction.x;
            if (data.m_correction.y > max.y)max.y = data.m_correction.y;
            if (data.m_correction.z > max.z)max.z = data.m_correction.z;
            
            if (data.m_mtv.y < minMtvY)minMtvY = data.m_mtv.y;
        }

        // 補正値へ統合
        XMFLOAT3 correction = { 0.0f,0.0f,0.0f };
        correction.x = min.x + max.x;
        correction.y = min.y + max.y;
        correction.z = min.z + max.z;

        // 外力に適用
        revVelocity.x += correction.x / deltaTime;
        revVelocity.y += correction.y / deltaTime;
        revVelocity.z += correction.z / deltaTime;

        // 接地判定
        if (minMtvY < 0.03f) {
            isGrounded = true;
        }
    }

    // 急激な反発を防止
    revVelocity.x = MiMath::Clamp(revVelocity.x, -REV_VELOCITY_MAX, REV_VELOCITY_MAX);
    revVelocity.y = MiMath::Clamp(revVelocity.y, -REV_VELOCITY_MAX, REV_VELOCITY_MAX);
    revVelocity.z = MiMath::Clamp(revVelocity.z, -REV_VELOCITY_MAX, REV_VELOCITY_MAX);

    // 位置と速度を算出
    XMFLOAT3   position = transform->GetPosition();
    position.x += revVelocity.x * deltaTime;
    position.y += revVelocity.y * deltaTime;
    position.z += revVelocity.z * deltaTime;

    XMFLOAT3   velocity = rigidbody->GetVelocity();
    velocity.x += revVelocity.x * 0.03f;
    velocity.y += revVelocity.y * 0.03f;
    velocity.z += revVelocity.z * 0.03f;

    // 動きが余りに小さかった場合
    // ・移動をリセットする
    // ・速度は0に近づける
    XMFLOAT3 prevPosition = transform->GetPrevPosition();
    if (abs(position.x - prevPosition.x) < 0.005f) {
        position.x = prevPosition.x;
        velocity.x *= 0.5f;
    }
    if (abs(position.y - prevPosition.y) < 0.002f) {
        position.y = prevPosition.y;
        velocity.y *= 0.5f;
    }
    if (abs(position.z - prevPosition.z) < 0.005f) {
        position.z = prevPosition.z;
        velocity.z *= 0.5f;
    }

    // 適用
    transform->SetPosition(position);
    rigidbody->SetVelocity(velocity);
    rigidbody->SetIsGrounded(isGrounded);
}
