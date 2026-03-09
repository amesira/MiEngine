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

            // 物理演算補正適用
            ApplyResolve(transform, collider, rigidbody, deltaTime);
        }
    }
}

void ResolvePass::ApplyResolve(TransformComponent* transform, ColliderComponent* collider, RigidbodyComponent* rigidbody, float deltaTime)
{
    bool isGrounded = false;

    // 外力を算出
    DirectX::XMFLOAT3 revVelocity = { 0.0f,0.0f,0.0f };
    {
        DirectX::XMFLOAT3   minMtv = collider->GetMinMtv();
        DirectX::XMFLOAT3   maxMtv = collider->GetMaxMtv();

        // minMtvが負、maxMtvが正の成分のみを採用する
        if (minMtv.x > 0.0f)minMtv.x = 0.0f;
        if (minMtv.y > 0.0f)minMtv.y = 0.0f;
        if (minMtv.z > 0.0f)minMtv.z = 0.0f;

        if (maxMtv.x < 0.0f)maxMtv.x = 0.0f;
        if (maxMtv.y < 0.0f)maxMtv.y = 0.0f;
        if (maxMtv.z < 0.0f)maxMtv.z = 0.0f;

        // 外力に適用
        revVelocity.x += (minMtv.x + maxMtv.x) / deltaTime;
        revVelocity.y += (minMtv.y + maxMtv.y) / deltaTime;
        revVelocity.z += (minMtv.z + maxMtv.z) / deltaTime;

        // 接地判定
        if (minMtv.y < 0.03f) {
            isGrounded = true;
        }
    }

    // 急激な反発を防止
    revVelocity.x = MiMath::Clamp(revVelocity.x, -REV_VELOCITY_MAX, REV_VELOCITY_MAX);
    revVelocity.y = MiMath::Clamp(revVelocity.y, -REV_VELOCITY_MAX, REV_VELOCITY_MAX);
    revVelocity.z = MiMath::Clamp(revVelocity.z, -REV_VELOCITY_MAX, REV_VELOCITY_MAX);

    // 位置と速度を算出
    DirectX::XMFLOAT3   position = transform->GetPosition();
    position.x += revVelocity.x * deltaTime;
    position.y += revVelocity.y * deltaTime;
    position.z += revVelocity.z * deltaTime;

    DirectX::XMFLOAT3   velocity = rigidbody->GetVelocity();
    velocity.x += revVelocity.x * 0.03f;
    velocity.y += revVelocity.y * 0.03f;
    velocity.z += revVelocity.z * 0.03f;

    // 動きが余りに小さかった場合
    // ・移動をリセットする
    // ・速度は0に近づける
    DirectX::XMFLOAT3 prevPosition = transform->GetPrevPosition();
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
