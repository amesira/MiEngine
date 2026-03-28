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
#include "pass.h"

#include "direct3d.h"
#include <DirectXMath.h>
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

private:
    // 連続衝突判定の速度閾値
    static constexpr float  VELOCITY_CCB_THRESHOLD = 200.0f;
    // 連続衝突判定の速度範囲
    static constexpr float  VELOCITY_CCB_RANGE = 70.0f;

    // 衝突判定結果
    struct CollisionResult {
        bool        isCollision;
        XMFLOAT3    mtv;
    };

    //---------------------------------------------------
    // 当たり判定の前段階の処理
    //---------------------------------------------------
    // レイヤーマスクによる当たり判定スキップ
    bool    IsIgnoreLayerPair(int layerA, int layerB);

    // CCBステップ数の計算
    int     CalculateCCBStep(RigidbodyComponent* rb, float deltaTime);

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
    void    CheckAABB(CollisionResult& result, Bounds a, Bounds b);

    //----------------------------------------------------
	// 当たり判定の本格チェック
    // ・コライダーの種類別に当たり判定を行う関数群
	//----------------------------------------------------
    // Box - Box
    void    CheckOBB(
        CollisionResult& result,
        TransformComponent* transformA, BoxColliderComponent* colliderA,
        TransformComponent* transformB, BoxColliderComponent* colliderB);
    // Box - Sphere
    void    CheckOBBSphere(
        CollisionResult& result,
        TransformComponent* transformA, BoxColliderComponent* colliderA,
        TransformComponent* transformB, SphereColliderComponent* colliderB);
    // Sphere - Sphere
    void    CheckSphere(
        CollisionResult& result,
        TransformComponent* transformA, SphereColliderComponent* colliderA,
        TransformComponent* transformB, SphereColliderComponent* colliderB);

    //----------------------------------------------------
    // 当たり判定後の処理
	//----------------------------------------------------
    // Correctionの作成
    void    CreateCorrection(
        XMFLOAT3& outCorrection, const XMFLOAT3& mtv, 
        ColliderComponent* collider, ColliderComponent* otherCollider,
        RigidbodyComponent* rb, RigidbodyComponent* otherRb);

    //----------------------------------------------------
    // デバッグ用コライダー描画
    void    DrawDebug_ColliderLine(TransformComponent* transform, BoxColliderComponent* boxCollider);
    void    DrawDebug_ColliderLine(TransformComponent* transform, SphereColliderComponent* sphereCollider);

};


#endif