//===================================================
// physics_processor.cpp [物理演算制御プロセッサー]
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "physics_processor.h"

#include "game_object.h"
#include "scene_interface.h"

#include "transform_component.h"
#include "rigidbody_component.h"

#include "mi_fps.h"

void PhysicsProcessor::Initialize()
{
    
}

void PhysicsProcessor::Finalize()
{

}

void PhysicsProcessor::Process(IScene* pScene)
{
    float deltaTime = FPS_GetDeltaTime();

    auto* rigidbodyPool = pScene->GetComponentPool<RigidbodyComponent>();
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    if (rigidbodyPool == nullptr || transformPool == nullptr)return;

    auto& rigidbodyList = rigidbodyPool->GetList();

    for (RigidbodyComponent& r : rigidbodyList) {
        TransformComponent* transform = transformPool->GetByGameObjectID(r.GetOwner()->GetID());
        RigidbodyComponent* rigidbody = &r;

        // コンポーネントが無効ならスキップ
        if (!transform || !rigidbody)continue;
        if (!transform->GetEnable() || !rigidbody->GetEnable())continue;

        transform->SetPrevPosition(transform->GetPosition());
        rigidbody->SetPrevVelocity(rigidbody->GetVelocity());

        DirectX::XMFLOAT3 velocity = rigidbody->GetVelocity();
        {
            // 摩擦の適用
            const DirectX::XMFLOAT3 friction = rigidbody->GetFriction();
            velocity.x *= friction.x;
            velocity.y *= friction.y;
            velocity.z *= friction.z;

            // 重力の適用
            velocity.y += rigidbody->GetGravityScale() * rigidbody->GetMass() * deltaTime;
        }
        rigidbody->SetVelocity(velocity);

        // 位置の更新
        DirectX::XMFLOAT3 position = transform->GetPosition();
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        position.z += velocity.z * deltaTime;
        transform->SetPosition(position);
    }
}
