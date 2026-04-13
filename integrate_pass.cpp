//===================================================
// integrate_pass.cpp [物理演算パス]
// 
// ・重力、摩擦などをvelocity（速度）に設定し、位置への適用を行う
// ・物理演算群の1番目にあたる。
// 
// Author：Miu Kitamura
// Date  ：2026/03/10
//===================================================
#include "integrate_pass.h"
#include "game_object.h"
#include "scene_interface.h"

#include "transform_component.h"
#include "rigidbody_component.h"

#include "mi_fps.h"

void IntegratePass::Initialize()
{
    
}

void IntegratePass::Finalize()
{

}

void IntegratePass::Process(IScene* pScene)
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

        // キネマティックなら速度を0にして位置の更新も行わない
        if (rigidbody->GetIsKinematic()) {
            rigidbody->SetVelocity({ 0.0f, 0.0f, 0.0f });
            continue;
        }

        XMFLOAT3 velocity = rigidbody->GetVelocity();
        {
            // 摩擦の適用
            const XMFLOAT3 friction = rigidbody->GetFriction();
            velocity.x *= friction.x;
            velocity.y *= friction.y;
            velocity.z *= friction.z;

            // 重力の適用
            velocity.y += rigidbody->GetGravityScale() * rigidbody->GetMass() * deltaTime;
        }
        rigidbody->SetVelocity(velocity);

        // 位置の更新
        XMFLOAT3 position = transform->GetPosition();
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        position.z += velocity.z * deltaTime;
        transform->SetPosition(position);
    }
}