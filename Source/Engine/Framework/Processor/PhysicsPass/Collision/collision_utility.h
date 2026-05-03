//---------------------------------------------------
// collision_utility.h [当たり判定ユーティリティ]
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//---------------------------------------------------
#ifndef COLLISION_UTILITY_H
#define COLLISION_UTILITY_H

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

class TransformComponent;
class ColliderComponent;
class RigidbodyComponent;

// コライダーの種類
class BoxColliderComponent;
class SphereColliderComponent;

// 衝突判定結果
struct CollisionResult {
    bool        isCollision;
    XMFLOAT3    mtv;
};

// AABB境界情報
struct Bounds {
    float   minX, maxX;
    float   minY, maxY;
    float   minZ, maxZ;
};

// Raycastのヒット情報
struct RaycastHit {
    bool        hit = false;
    GameObject* hitObject = nullptr;

    XMFLOAT3 hitPoint = { 0.0f, 0.0f, 0.0f };   // ヒットポイント
    XMFLOAT3 hitNormal = { 0.0f, 0.0f, 0.0f };  // ヒット法線
    float hitDistance = 0.0f;                   // ヒット距離
};

class CollisionUtility {
private:
    friend class CollisionPass;
    friend class CollisionQuery;

    // レイヤーマスクによる当たり判定スキップ
    static bool IsIgnoreLayerPair(int layerA, int layerB);

    //----------------------------------------------------
    // AABB境界情報の計算・判定
    //----------------------------------------------------
    // AABB境界情報の計算
    static Bounds ConvertToBounds(TransformComponent* transform, BoxColliderComponent* collider);
    static Bounds ConvertToBounds(TransformComponent* transform, SphereColliderComponent* collider);

    // AABB同士の衝突判定
    static void CheckAABB(/*out*/ CollisionResult& result, Bounds a, Bounds b);

    //----------------------------------------------------
    // 当たり判定の本格チェック
    // ・コライダーの種類別に当たり判定を行う関数群
    //----------------------------------------------------
    // Box - Box
    static void CheckOBB(
        /*out*/ CollisionResult& result,
        TransformComponent* transformA, BoxColliderComponent* colliderA,
        TransformComponent* transformB, BoxColliderComponent* colliderB);
    // Box - Sphere
    static void CheckOBBSphere(
        /*out*/ CollisionResult& result,
        TransformComponent* transformA, BoxColliderComponent* colliderA,
        TransformComponent* transformB, SphereColliderComponent* colliderB);
    // Sphere - Sphere
    static void CheckSphere(
        /*out*/ CollisionResult& result,
        TransformComponent* transformA, SphereColliderComponent* colliderA,
        TransformComponent* transformB, SphereColliderComponent* colliderB);

    //----------------------------------------------------
    // Rayの判定チェック
    //----------------------------------------------------
    // Ray - Box
    static void CheckRayOBB(
        /*out*/ RaycastHit& hitInfo,
        const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection,float rayLength,
        TransformComponent* transform, BoxColliderComponent* collider);
    // Ray - Sphere
    static void CheckRaySphere(
        /*out*/ RaycastHit& hitInfo,
        const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection,float rayLength,
        TransformComponent* transform, SphereColliderComponent* collider);
    // Ray - AABB
    static void CheckRayAABB(
        /*out*/ RaycastHit& hitInfo,
        const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection,float rayLength,
        Bounds bounds);
};

#endif // COLLISION_UTILITY_H