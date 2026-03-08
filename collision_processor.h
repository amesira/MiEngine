//----------------------------------------------------
// collision_processor.h [当たり判定プロセッサー]
// 
// ・当たり判定を取る。
// ・物理処理群の２番目。
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//----------------------------------------------------
#ifndef COLLISION_PROCESSOR_H
#define COLLISION_PROCESSOR_H

#include <vector>
#include "processor.h"

#include "direct3d.h"
#include <DirectXMath.h>

class TransformComponent;
class ColliderComponent;

class BoxColliderComponent;
class SphereColliderComponent;

class CollisionProcessor : public Processor {
public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;

private:
    void    DrawDebugCollider(IScene* pScene);

    // 衝突判定結果
    struct CollisionResult {
        bool                isCollision;
        DirectX::XMFLOAT3   mtv;
    };
    
    //----------------------------------------------------
    // AABB境界情報の計算・判定
	//----------------------------------------------------
    // AABB境界情報
    struct Bounds {
        float   minX, maxX;
        float   minY, maxY;
        float   minZ, maxZ;
    };

    // AABB境界情報の計算
    Bounds  ConvertToBounds(TransformComponent* transform, BoxColliderComponent* collider);
    Bounds  ConvertToBounds(TransformComponent* transform, SphereColliderComponent* collider);

    // AABB同士の衝突判定
    CollisionResult    CheckAABB(Bounds a, Bounds b);

    //----------------------------------------------------
	// 詳細な衝突判定
	//----------------------------------------------------
    CollisionResult     CheckBoxToBox(
        TransformComponent* transformA, BoxColliderComponent* colliderA,
        TransformComponent* transformB, BoxColliderComponent* colliderB);
    CollisionResult     CheckBoxToSphere(
        TransformComponent* transformA, BoxColliderComponent* colliderA,
        TransformComponent* transformB, SphereColliderComponent* colliderB);
    CollisionResult     CheckSphereToSphere(
        TransformComponent* transformA, SphereColliderComponent* colliderA,
        TransformComponent* transformB, SphereColliderComponent* colliderB);


};


#endif