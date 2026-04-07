//===================================================
// physics_processor.cpp [物理演算制御プロセッサー]
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "physics_processor.h"
#include "game_object.h"
#include "scene_interface.h"

#include "mi_fps.h"

#include "transform_component.h"
#include "rigidbody_component.h"

void PhysicsProcessor::Initialize()
{
    // 各Passの初期化
    m_integratePass.Initialize();
    m_collisionPass.Initialize();
    m_resolvePass.Initialize();
}

void PhysicsProcessor::Finalize()
{
    // 各Passの終了処理
    m_integratePass.Finalize();
    m_collisionPass.Finalize();
    m_resolvePass.Finalize();
}

void PhysicsProcessor::Process(IScene* pScene)
{
    // 前フレームの状態保持
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto& transformList = transformPool->GetList();
    for (TransformComponent& t : transformList) {
        TransformComponent* transform = &t;
        if (!transform || !transform->GetEnable())continue;
        transform->SetPrevPosition(transform->GetPosition());
    }

    auto* rigidbodyPool = pScene->GetComponentPool<RigidbodyComponent>();
    auto& rigidbodyList = rigidbodyPool->GetList();
    for (RigidbodyComponent& r : rigidbodyList) {
        RigidbodyComponent* rigidbody = &r;
        if (!rigidbody || !rigidbody->GetEnable())continue;
        rigidbody->SetPrevVelocity(rigidbody->GetVelocity());
    }

    // 重力・外力を位置へ適用
    m_integratePass.Process(pScene);
    
    // 位置から衝突判定
    m_collisionPass.Process(pScene);

    // 衝突解決・演算補正
    m_resolvePass.Process(pScene);

    // 拘束解決
    m_constraintPass.Process(pScene);

}
