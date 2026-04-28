//===================================================
// collision_query.h [当たり判定クエリー]
// ・RaycastやSphereCastなど、即時実行型の当たり判定クエリーを提供するクラス。
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//===================================================
#include "collision_query.h"
#include "collision_utility.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"

#include "Engine/System/Device/mi_fps.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/collider_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"

#include "Utility/debug_renderer.h"
#include "Utility/mi_math.h"

//===================================================
// RayCastクエリー
//===================================================
// Raycastクエリー
bool CollisionQuery::Raycast(IScene* scene, RaycastHit& raycastHit, 
    const XMFLOAT3& origin, const XMFLOAT3& direction, float maxDistance, int layerMask)
{
    return false;
}

// SphereCastクエリー
bool CollisionQuery::SphereCast(IScene* scene, RaycastHit& raycastHit, 
    const XMFLOAT3& origin, const XMFLOAT3& direction, float radius, float maxDistance, int layerMask)
{
    return false;
}

//===================================================
// Overlapクエリ―
//===================================================
// OverlapBoxクエリー
bool CollisionQuery::OverlapBox(IScene* scene, std::vector<ColliderComponent*>& outObjects, 
    const XMFLOAT3& center, const XMFLOAT3& scale, const XMFLOAT4& orientation, int layerMask)
{
    auto* boxColliderPools = scene->GetComponentPool<BoxColliderComponent>();
    auto* sphereColliderPools = scene->GetComponentPool<SphereColliderComponent>();
    auto* transformPool = scene->GetComponentPool<TransformComponent>();

    // 判定用の一時Componentを作成
    TransformComponent tempTransform;
    BoxColliderComponent tempBoxCollider;
    {
        tempTransform.SetPosition(center);
        tempTransform.SetRotation(orientation);
        tempBoxCollider.SetScale(scale);
        tempBoxCollider.SetLayer(ColliderComponent::Layer::Default); // ToDo: レイヤーの指定方法を検討
    }

    if (boxColliderPools) {
        auto& boxColliders = boxColliderPools->GetList();

        for (BoxColliderComponent& boxCollider : boxColliders) {
            if (!boxCollider.GetEnable()) continue;
            if (CollisionUtility::IsIgnoreLayerPair(layerMask, (int)boxCollider.GetLayer())) continue;
            
            TransformComponent* t = transformPool->GetByGameObjectID(boxCollider.GetOwner()->GetID());
            
            if (t == nullptr) continue;
            
            CollisionUtility::CollisionResult result;
            CollisionUtility::CheckOBB(
                /*out*/ result,
                t, &boxCollider,
                &tempTransform, &tempBoxCollider
            );

            if (result.isCollision) {
                outObjects.push_back(&boxCollider);
            }
        }
    }

    if (sphereColliderPools) {
        auto& sphereColliders = sphereColliderPools->GetList();
    
        for (SphereColliderComponent& sphereCollider : sphereColliders) {
            if (!sphereCollider.GetEnable()) continue;
            if (CollisionUtility::IsIgnoreLayerPair(layerMask, (int)sphereCollider.GetLayer())) continue;
    
            TransformComponent* t = transformPool->GetByGameObjectID(sphereCollider.GetOwner()->GetID());
    
            if (t == nullptr) continue;
    
            CollisionUtility::CollisionResult result;
            CollisionUtility::CheckOBBSphere(
                /*out*/ result,
                &tempTransform, &tempBoxCollider,
                t, &sphereCollider
            );
    
            if (result.isCollision) {
                outObjects.push_back(&sphereCollider);
            }
        }
    }
}

// OverlapSphereクエリー
bool CollisionQuery::OverlapSphere(IScene* scene, std::vector<ColliderComponent*>& outObjects, 
    const XMFLOAT3& center, float radius, int layerMask)
{
    auto* boxColliderPools = scene->GetComponentPool<BoxColliderComponent>();
    auto* sphereColliderPools = scene->GetComponentPool<SphereColliderComponent>();
    auto* transformPool = scene->GetComponentPool<TransformComponent>();

    // 判定用の一時Componentを作成
    TransformComponent tempTransform;
    SphereColliderComponent tempSphereCollider;
    {
        tempTransform.SetPosition(center);
        tempSphereCollider.SetRadius(radius);
        tempSphereCollider.SetLayer(ColliderComponent::Layer::Default); // ToDo: レイヤーの指定方法を検討
    }

    if (boxColliderPools) {
        auto& boxColliders = boxColliderPools->GetList();
        for (BoxColliderComponent& boxCollider : boxColliders) {
            if (!boxCollider.GetEnable()) continue;
            if (CollisionUtility::IsIgnoreLayerPair(layerMask, (int)boxCollider.GetLayer())) continue;
            
            TransformComponent* t = transformPool->GetByGameObjectID(boxCollider.GetOwner()->GetID());
            
            if (t == nullptr) continue;
            
            CollisionUtility::CollisionResult result;
            CollisionUtility::CheckOBBSphere(
                /*out*/ result,
                t, &boxCollider,
                &tempTransform, &tempSphereCollider
            );

            if (result.isCollision) {
                outObjects.push_back(&boxCollider);
            }
        }
    }

    if (sphereColliderPools) {
        auto& sphereColliders = sphereColliderPools->GetList();
        for (SphereColliderComponent& sphereCollider : sphereColliders) {
            if (!sphereCollider.GetEnable()) continue;
            if (CollisionUtility::IsIgnoreLayerPair(layerMask, (int)sphereCollider.GetLayer())) continue;
            
            TransformComponent* t = transformPool->GetByGameObjectID(sphereCollider.GetOwner()->GetID());
            
            if (t == nullptr) continue;
            
            CollisionUtility::CollisionResult result;
            CollisionUtility::CheckSphere(
                /*out*/ result,
                t, &sphereCollider,
                &tempTransform, &tempSphereCollider
            );

            if (result.isCollision) {
                outObjects.push_back(&sphereCollider);
            }
        }
    }
}