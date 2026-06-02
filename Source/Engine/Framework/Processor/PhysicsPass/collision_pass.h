//----------------------------------------------------
// collision_pass.h [当たり判定パス]
// 
// ・当たり判定を取る。
// ・物理処理群の２番目。
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//----------------------------------------------------
#ifndef COLLISION_PASS_H
#define COLLISION_PASS_H
#include "Engine/Core/pass.h"

#include "Engine/Device/direct3d.h"
using namespace DirectX;

class TransformComponent;
class ColliderComponent;
class RigidbodyComponent;

// コライダーの種類
class BoxColliderComponent;
class SphereColliderComponent;

class CollisionPass : public Pass {
public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;
    void    CollectDebugDraw(IScene* pScene);

private:
    // デバッグ用コライダー描画
    void    DrawDebug_ColliderLine(TransformComponent* transform, BoxColliderComponent* boxCollider);
    void    DrawDebug_ColliderLine(TransformComponent* transform, SphereColliderComponent* sphereCollider);

};


#endif
