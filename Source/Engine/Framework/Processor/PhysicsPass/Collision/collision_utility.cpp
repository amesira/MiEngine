//===================================================
// collision_utility.cpp [当たり判定ユーティリティ]
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//===================================================
#include "collision_utility.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/collider_component.h"
#include "Engine/Core/game_object_layer.h"

#include "Utility/mi_math.h"

#include <algorithm>
#include <cmath>

using namespace DirectX;

constexpr bool COLLISION_MATRIX[(int)CollisionLayer::MAX][(int)CollisionLayer::MAX] = {
    //                 Default    Field      Player     Bullet        Enemy
    /*Default   */   { true,      true,      true,      false,       true},
    /*Field     */   { true,      false,     true,      true,      true},
    /*Player    */   { true,      true,      true,      false,       true},
    /*Bullet      */   { false,    false,    false,     true,       true},
    /*Enemy     */   { true,      true,      true,      true,       true},

};

// レイヤーマスクによる当たり判定スキップ
bool CollisionUtility::IsIgnoreLayerPair(int layerA, int layerB)
{
    if (layerA < 0 || layerA >= (int)CollisionLayer::MAX ||
        layerB < 0 || layerB >= (int)CollisionLayer::MAX) {
        return false; // 無効なレイヤー番号の場合はスキップしない
    }

    if (!COLLISION_MATRIX[layerA][layerB]) {
        return true;
    }
    return false;
}

#pragma region AABB境界情報の計算・判定
// BoxColliderのAABB境界情報計算
Bounds CollisionUtility::ConvertToBounds(
    TransformComponent* t, BoxColliderComponent* c)
{
    Bounds bounds = {};

    XMMATRIX R = XMMatrixRotationQuaternion(t->GetRotationVector());

    // half extents (ローカル半サイズ)
    float ex = c->GetScale().x * 0.5f;
    float ey = c->GetScale().y * 0.5f;
    float ez = c->GetScale().z * 0.5f;

    // ワールド座標系での中心座標を計算
    XMFLOAT3 pos = MiMath::RotateVector(t->GetRotation(), c->GetCenter());
    pos = MiMath::Add(pos, t->GetPosition());

    // abs(R) * e
    XMFLOAT3 aabbExtents = {
        ex * fabsf(XMVectorGetX(R.r[0])) + ey * fabsf(XMVectorGetX(R.r[1])) + ez * fabsf(XMVectorGetX(R.r[2])),
        ex * fabsf(XMVectorGetY(R.r[0])) + ey * fabsf(XMVectorGetY(R.r[1])) + ez * fabsf(XMVectorGetY(R.r[2])),
        ex * fabsf(XMVectorGetZ(R.r[0])) + ey * fabsf(XMVectorGetZ(R.r[1])) + ez * fabsf(XMVectorGetZ(R.r[2])),
    };

    bounds.minX = pos.x - aabbExtents.x;
    bounds.maxX = pos.x + aabbExtents.x;
    bounds.minY = pos.y - aabbExtents.y;
    bounds.maxY = pos.y + aabbExtents.y;
    bounds.minZ = pos.z - aabbExtents.z;
    bounds.maxZ = pos.z + aabbExtents.z;

    return bounds;
}

// SphereColliderのAABB境界情報計算
Bounds CollisionUtility::ConvertToBounds(TransformComponent* t, SphereColliderComponent* c)
{
    Bounds bounds = {};

    // ワールド座標系での中心座標・サイズを計算
    DirectX::XMFLOAT3 pos = {
        t->GetPosition().x + c->GetCenter().x,
        t->GetPosition().y + c->GetCenter().y,
        t->GetPosition().z + c->GetCenter().z
    };
    float radius = c->GetRadius();

    // 頂点の内、最も小さいもの大きいものを計算
    bounds.minX = pos.x - radius;
    bounds.maxX = pos.x + radius;
    bounds.minY = pos.y - radius;
    bounds.maxY = pos.y + radius;
    bounds.minZ = pos.z - radius;
    bounds.maxZ = pos.z + radius;

    return bounds;
}

// AABB同士の衝突判定
void CollisionUtility::CheckAABB(CollisionResult& result, Bounds a, Bounds b)
{
    // 衝突判定
    result.isCollision = (
        a.minX <= b.maxX &&
        a.maxX >= b.minX &&
        a.minY <= b.maxY &&
        a.maxY >= b.minY &&
        a.minZ <= b.maxZ &&
        a.maxZ >= b.minZ
        );

    // Aの最小移動ベクトル
    // ・BとAが重なっている場合、AをBの外に出すためにAが移動すべき最小のベクトル
    result.mtv = { 0.0f,0.0f,0.0f };
    if (result.isCollision) {
        float dx = (a.minX < b.minX) ? (b.minX - a.maxX) : (b.maxX - a.minX);
        float dy = (a.minY < b.minY) ? (b.minY - a.maxY) : (b.maxY - a.minY);
        float dz = (a.minZ < b.minZ) ? (b.minZ - a.maxZ) : (b.maxZ - a.minZ);

        // XYZの中で必要な押し出し量が最も小さい軸を採用
        if (abs(dx) < abs(dy) && abs(dx) < abs(dz)) {
            result.mtv.x = dx;
        }
        else if (abs(dy) < abs(dz)) {
            result.mtv.y = dy;
        }
        else {
            result.mtv.z = dz;
        }
    }
}
#pragma endregion

#pragma region 当たり判定の本格チェック
// Box同士の衝突判定
void CollisionUtility::CheckOBB(
    /*out*/ CollisionResult& result,
    TransformComponent* tA, BoxColliderComponent* cA,
    TransformComponent* tB, BoxColliderComponent* cB)
{
    result.isCollision = false;

    // コライダーのワールド座標を取得
    XMFLOAT3 posA = MiMath::RotateVector(tA->GetRotation(), cA->GetCenter());
    posA.x += tA->GetPosition().x;
    posA.y += tA->GetPosition().y;
    posA.z += tA->GetPosition().z;
    XMFLOAT3 posB = MiMath::RotateVector(tB->GetRotation(), cB->GetCenter());
    posB.x += tB->GetPosition().x;
    posB.y += tB->GetPosition().y;
    posB.z += tB->GetPosition().z;

    // 中心点間のベクトル
    XMFLOAT3 diff = {
        posB.x - posA.x,
        posB.y - posA.y,
        posB.z - posA.z
    };

    // 分離軸の情報
    XMFLOAT3 ea1 = {cA->GetScale().x * 0.5f, 0.0f, 0.0f};
    ea1 = MiMath::RotateVector(tA->GetRotation(), ea1);
    XMFLOAT3 ea2 = { 0.0f, cA->GetScale().y * 0.5f, 0.0f };
    ea2 = MiMath::RotateVector(tA->GetRotation(), ea2);
    XMFLOAT3 ea3 = { 0.0f, 0.0f, cA->GetScale().z * 0.5f };
    ea3 = MiMath::RotateVector(tA->GetRotation(), ea3);

    XMFLOAT3 eb1 = { cB->GetScale().x * 0.5f, 0.0f, 0.0f };
    eb1 = MiMath::RotateVector(tB->GetRotation(), eb1);
    XMFLOAT3 eb2 = { 0.0f, cB->GetScale().y * 0.5f, 0.0f };
    eb2 = MiMath::RotateVector(tB->GetRotation(), eb2);
    XMFLOAT3 eb3 = { 0.0f, 0.0f, cB->GetScale().z * 0.5f };
    eb3 = MiMath::RotateVector(tB->GetRotation(), eb3);

    XMFLOAT3 c11 = MiMath::Cross(ea1, eb1);
    XMFLOAT3 c12 = MiMath::Cross(ea1, eb2);
    XMFLOAT3 c13 = MiMath::Cross(ea1, eb3);

    XMFLOAT3 c21 = MiMath::Cross(ea2, eb1);
    XMFLOAT3 c22 = MiMath::Cross(ea2, eb2);
    XMFLOAT3 c23 = MiMath::Cross(ea2, eb3);

    XMFLOAT3 c31 = MiMath::Cross(ea3, eb1);
    XMFLOAT3 c32 = MiMath::Cross(ea3, eb2);
    XMFLOAT3 c33 = MiMath::Cross(ea3, eb3);

    XMFLOAT3 L[15] = {
        ea1, ea2, ea3,
        eb1, eb2, eb3,
        c11, c12, c13,
        c21, c22, c23,
        c31, c32, c33
    };

    // mtv用の保持変数
    float minOverlap = 1000000.0f;
    XMFLOAT3 mtvAxis = { 0.0f,0.0f,0.0f };

    //----------------------------------------------------
    // 衝突判定処理
	//----------------------------------------------------
    for(int i = 0; i < 15; i++){
        if (MiMath::Length(L[i]) < 0.001f) continue;

        XMFLOAT3 l = MiMath::Normalize(L[i]);

        // 中心点間の距離を投影
        float interval = abs(MiMath::Dot(diff, l));

        // 半径を投影
        float rA =
            fabsf(MiMath::Dot(ea1, l)) +
            fabsf(MiMath::Dot(ea2, l)) +
            fabsf(MiMath::Dot(ea3, l));
        float rB =
            fabsf(MiMath::Dot(eb1, l)) +
            fabsf(MiMath::Dot(eb2, l)) +
            fabsf(MiMath::Dot(eb3, l));

        // 衝突判定
        if (interval > (rA + rB)) {
            // 衝突していない
            result.isCollision = false;
            return;
        }

        // 最小移動ベクトルの計算
        if(minOverlap > (rA + rB) - interval){
            minOverlap = rA + rB - interval;
            mtvAxis = l;
        }
    }

    //----------------------------------------------------
    // resultの設定
	//----------------------------------------------------
    result.isCollision = true;

    // mtvの設定
    if (MiMath::Dot(diff, mtvAxis) > 0.0f) { // 方向を反転
        mtvAxis = {
            -mtvAxis.x,
            -mtvAxis.y,
            -mtvAxis.z
        };
    }
    result.mtv = {
        mtvAxis.x * minOverlap,
        mtvAxis.y * minOverlap,
        mtvAxis.z * minOverlap
    };
}

// BoxとSphereの衝突判定
void CollisionUtility::CheckOBBSphere(
    /*out*/ CollisionResult& result,
    TransformComponent* tA, BoxColliderComponent* cA,
    TransformComponent* tB, SphereColliderComponent* cB)
{
    // ワールド座標系での中心座標を計算
    XMFLOAT3 boxPos = MiMath::RotateVector(tA->GetRotation(), cA->GetCenter());
    boxPos.x += tA->GetPosition().x;
    boxPos.y += tA->GetPosition().y;
    boxPos.z += tA->GetPosition().z;
    XMFLOAT3 spherePos = MiMath::RotateVector(tB->GetRotation(), cB->GetCenter());
    spherePos.x += tB->GetPosition().x;
    spherePos.y += tB->GetPosition().y;
    spherePos.z += tB->GetPosition().z;

    // BoxColliderから見たSphereColliderのローカル座標を計算
    // ・BoxColliderをAABBとして扱うため
    XMFLOAT3 localSpherePos = MiMath::RotateVector(
        XMFLOAT4(
            -tA->GetRotation().x,
            -tA->GetRotation().y,
            -tA->GetRotation().z,
            tA->GetRotation().w
        ),
        {
            spherePos.x - boxPos.x,
            spherePos.y - boxPos.y,
            spherePos.z - boxPos.z
        });

    // AABBの各軸に沿った最近接点を計算
    XMFLOAT3 halfExtents = {
        cA->GetScale().x * 0.5f,
        cA->GetScale().y * 0.5f,
        cA->GetScale().z * 0.5f
    };
    XMFLOAT3 closestPoint = {
        MiMath::Clamp(localSpherePos.x, -halfExtents.x, halfExtents.x),
        MiMath::Clamp(localSpherePos.y, -halfExtents.y, halfExtents.y),
        MiMath::Clamp(localSpherePos.z, -halfExtents.z, halfExtents.z)
    };

    // 円の方程式による衝突判定
    XMFLOAT3 difference = {
        closestPoint.x - localSpherePos.x,
        closestPoint.y - localSpherePos.y,
        closestPoint.z - localSpherePos.z
    };
    float distanceSquared = {
        MiMath::Pow(difference.x, 2) +
        MiMath::Pow(difference.y, 2) +
        MiMath::Pow(difference.z, 2)
    };

    float radius = cB->GetRadius();

    if (distanceSquared < MiMath::Pow(radius, 2)) {
        // 衝突している
        result.isCollision = true;

        // 最小移動ベクトルの計算
        float distance = sqrtf(distanceSquared);
        float overlap = radius - distance;

        // ローカル座標系での最小移動ベクトル
        XMFLOAT3 localMtv = MiMath::Normalize(difference);
        localMtv = {
            localMtv.x * overlap,
            localMtv.y * overlap,
            localMtv.z * overlap
        };

        // ワールド座標系に変換
        XMFLOAT3 worldMtv = MiMath::RotateVector(
            tA->GetRotation(),
            localMtv
        );

        result.mtv = worldMtv;
    }
}

// Sphere同士の衝突判定
void CollisionUtility::CheckSphere(
    /*out*/ CollisionResult& result,
    TransformComponent* tA, SphereColliderComponent* cA,
    TransformComponent* tB, SphereColliderComponent* cB)
{
    // ワールド座標系での中心座標、半径を計算
    DirectX::XMFLOAT3 posA = MiMath::RotateVector(tA->GetRotation(), cA->GetCenter());
    posA.x += tA->GetPosition().x;
    posA.y += tA->GetPosition().y;
    posA.z += tA->GetPosition().z;
    DirectX::XMFLOAT3 posB = MiMath::RotateVector(tB->GetRotation(), cB->GetCenter());
    posB.x += tB->GetPosition().x;
    posB.y += tB->GetPosition().y;
    posB.z += tB->GetPosition().z;

    float radiusA = cA->GetRadius();
    float radiusB = cB->GetRadius();

    // コライダー間の距離の二乗を計算
    float interval = {
        MiMath::Pow((posB.x - posA.x), 2) +
        MiMath::Pow((posB.y - posA.y), 2) +
        MiMath::Pow((posB.z - posA.z), 2)
    };

    // 半径の和の二乗を計算
    float radiusSum = MiMath::Pow((radiusA + radiusB), 2);

    // 衝突判定
    if (interval <= radiusSum) {
        // 衝突している
        result.isCollision = true;

        // 最小移動ベクトルの計算
        // ・Aが移動すべき最小のベクトルを計算
        float i = sqrtf(interval);
        float overlap = i - (radiusA + radiusB);
        DirectX::XMFLOAT3 direction = {
            (posA.x - posB.x) / i,
            (posA.y - posB.y) / i,
            (posA.z - posB.z) / i
        };
        result.mtv = {
            -direction.x * overlap,
            -direction.y * overlap,
            -direction.z * overlap
        };
    }
}

#pragma endregion

#pragma region Rayの判定チェック
// RayとBoxの衝突判定
void CollisionUtility::CheckRayOBB(
    /*out*/ RaycastHit& hitInfo,
    const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection,float rayLength,
    TransformComponent* transform, BoxColliderComponent* collider)
{
    hitInfo = RaycastHit();

    // ワールド座標系での中心座標を計算
    XMFLOAT3 center = MiMath::RotateVector(transform->GetRotation(), collider->GetCenter());
    center = MiMath::Add(center, transform->GetPosition());

    // レイをOBBを無回転とした時のローカル座標系に変換
    XMFLOAT3 localRayOrigin = MiMath::RotateVector(
        XMFLOAT4(
            -transform->GetRotation().x,
            -transform->GetRotation().y,
            -transform->GetRotation().z,
            transform->GetRotation().w
        ),
        MiMath::Subtract(rayOrigin, center)
    );
    XMFLOAT3 localRayDirection = MiMath::RotateVector(
        XMFLOAT4(
            -transform->GetRotation().x,
            -transform->GetRotation().y,
            -transform->GetRotation().z,
            transform->GetRotation().w
        ),
        rayDirection
    );
    localRayDirection = MiMath::Normalize(localRayDirection);

    // AABBとの衝突判定
    Bounds bounds = {
        -collider->GetScale().x * 0.5f, collider->GetScale().x * 0.5f,
        -collider->GetScale().y * 0.5f, collider->GetScale().y * 0.5f,
        -collider->GetScale().z * 0.5f, collider->GetScale().z * 0.5f
    };

    RaycastHit localHitInfo;
    CheckRayAABB(
        /*out*/ localHitInfo,
        localRayOrigin,
        localRayDirection,
        rayLength,
        bounds
    );

    if (localHitInfo.hit) {
        // 衝突している
        hitInfo.hit = true;
        hitInfo.hitDistance = localHitInfo.hitDistance;
        hitInfo.hitPoint = MiMath::Add(rayOrigin, MiMath::Multiply(MiMath::Normalize(rayDirection), localHitInfo.hitDistance));
        hitInfo.hitNormal = MiMath::RotateVector(transform->GetRotation(), localHitInfo.hitNormal);
    }
    else {
        // 衝突していない
        hitInfo.hit = false;
    }
}

// RayとSphereの衝突判定
void CollisionUtility::CheckRaySphere(
    /*out*/ RaycastHit& hitInfo,
    const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection,const float rayLength,
    TransformComponent* transform, SphereColliderComponent* collider)
{
    hitInfo = RaycastHit();

    XMFLOAT3 dir = MiMath::Normalize(rayDirection);

    // レイの始点から球の中心へのベクトル
    XMFLOAT3 center = MiMath::RotateVector(transform->GetRotation(), collider->GetCenter());
    center = MiMath::Add(center, transform->GetPosition());
    XMFLOAT3 co = MiMath::Subtract(center, rayOrigin);

    float a = MiMath::Dot(dir, dir);
    float b = MiMath::Dot(co, dir);
    float c = MiMath::Dot(co, co) - MiMath::Pow(collider->GetRadius(), 2);

    if (a == 0.0f) {
        // レイの方向ベクトルがゼロの場合は衝突なし
        hitInfo.hit = false;
        return;
    }

    float s = b * b - a * c;
    if (s < 0.0f) {
        // 判別式が負の場合は衝突なし
        hitInfo.hit = false;
        return;
    }

    // 衝突点までの距離を計算
    float sqrtS = sqrtf(s);
    float a1 = (b - sqrtS) / a;
    float a2 = (b + sqrtS) / a;

    // レイの後方で衝突
    if (a1 < 0.0f && a2 < 0.0f) {
        hitInfo.hit = false;
        return;
    }

    // レイの長さを超える距離で衝突
    if (a1 > rayLength && a2 > rayLength) {
        hitInfo.hit = false;
        return;
    }

    // 衝突している
    hitInfo.hit = true;
    float a3 = (a1 >= 0.0f) ? a1 : a2;
    hitInfo.hitDistance = a3;
    hitInfo.hitPoint = MiMath::Add(rayOrigin, MiMath::Multiply(dir, a3));
    hitInfo.hitNormal = MiMath::Normalize(MiMath::Subtract(hitInfo.hitPoint, center));
}

// RayとAABBの衝突判定
void CollisionUtility::CheckRayAABB(
    /*out*/ RaycastHit& hitInfo,
    const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection, float rayLength,
    Bounds bounds)
{
    hitInfo = RaycastHit{};

    const XMFLOAT3 dir = MiMath::Normalize(rayDirection);
    const float epsilon = 0.000001f;
    float tMin = 0.0f;
    float tMax = rayLength;
    XMFLOAT3 hitNormal = { 0.0f, 0.0f, 0.0f };

    // 各軸のスラブを更新するラムダ関数
    auto updateSlab = [&](float origin, float direction, float minValue, float maxValue, const XMFLOAT3& minNormal, const XMFLOAT3& maxNormal) -> bool
        {
            if (std::fabs(direction) < epsilon) {
                return origin >= minValue && origin <= maxValue;
            }

            float t1 = (minValue - origin) / direction;
            float t2 = (maxValue - origin) / direction;
            XMFLOAT3 nearNormal = minNormal;
            XMFLOAT3 farNormal = maxNormal;

            if (t1 > t2) {
                std::swap(t1, t2);
                std::swap(nearNormal, farNormal);
            }

            if (t1 > tMin) {
                tMin = t1;
                hitNormal = nearNormal;
            }
            tMax = (std::min)(tMax, t2);

            return tMin <= tMax;
        };

    if (!updateSlab(rayOrigin.x, dir.x, bounds.minX, bounds.maxX, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f })) return;
    if (!updateSlab(rayOrigin.y, dir.y, bounds.minY, bounds.maxY, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f })) return;
    if (!updateSlab(rayOrigin.z, dir.z, bounds.minZ, bounds.maxZ, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f })) return;

    const float hitDistance = tMin >= 0.0f ? tMin : tMax;
    if (hitDistance < 0.0f || hitDistance > rayLength) return;

    hitInfo.hit = true;
    hitInfo.hitDistance = hitDistance;
    hitInfo.hitPoint = MiMath::Add(rayOrigin, MiMath::Multiply(dir, hitDistance));
    hitInfo.hitNormal = hitNormal;
}
#pragma endregion
