//===================================================
// collision_pass.cpp [当たり判定プロセッサー]
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "collision_pass.h"

#include "game_object.h"
#include "scene_interface.h"
#include "mi_math.h"

#include "transform_component.h"
#include "collider_component.h"

#include "debug_renderer.h"

using namespace DirectX;

// 衝突判定マトリクス
constexpr bool COLLISION_MATRIX[(int)ColliderComponent::Layer::MAX][(int)ColliderComponent::Layer::MAX] = {
    //                 Default    Field      Player     PlayerOnBall    Ball        Enemy
    /*Default   */   { true,      true,      true,      true,           true,       true},
    /*Field     */   { true,      false,     true,      true,           true,      true},
    /*Player    */   { true,      true,      true,      false,          true,       true},
    /*PlayerOnBall*/{ true,      true,      false,     false,          false,       true},
    /*Ball      */   { true,      true,     true,      false,           true,       true},
    /*Enemy     */   { true,      true,      true,      true,           true,       true},

};

void CollisionPass::Initialize()
{
    
}

void CollisionPass::Finalize()
{

}

void CollisionPass::Process(IScene* pScene)
{
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* boxColliderPool = pScene->GetComponentPool<BoxColliderComponent>();
    auto* sphereColliderPool = pScene->GetComponentPool<SphereColliderComponent>();
    if(transformPool == nullptr)return;

    if(boxColliderPool){
        auto& boxColliderList = boxColliderPool->GetList();
        // 衝突情報の更新
        for (BoxColliderComponent& c : boxColliderList) {
            c.UpdateCollisionData();
        }
    
        //----------------------------------------------------
        // 当たり判定処理
	    //----------------------------------------------------
    #pragma region CollisionDetection
        // BoxCollider同士の当たり判定
        for (int i = 0; i < boxColliderList.size(); i++) {
            BoxColliderComponent* colliderA = &boxColliderList[i];
            TransformComponent* transformA = transformPool->GetByGameObjectID(colliderA->GetOwner()->GetID());

            if (colliderA == nullptr || transformA == nullptr) continue;
            if (!colliderA->GetEnable() || !transformA->GetEnable()) continue;
            

            for (int j = 0; j < boxColliderList.size() - (i + 1); j++) {
                BoxColliderComponent* colliderB = &boxColliderList[i + (j + 1)];
                TransformComponent* transformB = transformPool->GetByGameObjectID(colliderB->GetOwner()->GetID());

                if (colliderB == nullptr || transformB == nullptr) continue;
                if (!colliderB->GetEnable() || !transformB->GetEnable()) continue;

                // レイヤーマスクによる当たり判定スキップ
                if (!COLLISION_MATRIX[(int)colliderA->GetLayer()][(int)colliderB->GetLayer()]) continue;

                // 詳細な衝突判定
                CollisionResult result = CheckBoxToBox(
                    transformA, colliderA,
                    transformB, colliderB);

                // 衝突している場合
                if (result.isCollision) {
                    colliderA->RegisterCollisionData(colliderB, result.mtv);
                    colliderB->RegisterCollisionData(colliderA, {
                        -result.mtv.x,
                        -result.mtv.y,
                        -result.mtv.z });
                }
            }
        }
    }
    
    if(sphereColliderPool){
        auto& sphereColliderList = sphereColliderPool->GetList();
        for (SphereColliderComponent& c : sphereColliderList) {
            c.UpdateCollisionData();
        }

        // SphereCollider同士の当たり判定
        for(int i = 0; i < sphereColliderList.size(); i++) {
            SphereColliderComponent* colliderA = &sphereColliderList[i];
            TransformComponent* transformA = transformPool->GetByGameObjectID(colliderA->GetOwner()->GetID());

            if (colliderA == nullptr || transformA == nullptr) continue;
            if (!colliderA->GetEnable() || !transformA->GetEnable()) continue;

            for (int j = 0; j < sphereColliderList.size() - (i + 1); j++) {
                SphereColliderComponent* colliderB = &sphereColliderList[i + (j + 1)];
                TransformComponent* transformB = transformPool->GetByGameObjectID(colliderB->GetOwner()->GetID());

                if (colliderB == nullptr || transformB == nullptr) continue;
                if (!colliderB->GetEnable() || !transformB->GetEnable()) continue;

                // レイヤーマスクによる当たり判定スキップ
                if (!COLLISION_MATRIX[(int)colliderA->GetLayer()][(int)colliderB->GetLayer()]) continue;

                // 詳細な衝突判定
                CollisionResult result = CheckSphereToSphere(
                    transformA, colliderA,
                    transformB, colliderB);

                // 衝突している場合
                if (result.isCollision) {
                    colliderA->RegisterCollisionData(colliderB, result.mtv);
                    colliderB->RegisterCollisionData(colliderA, {
                        -result.mtv.x,
                        -result.mtv.y,
                        -result.mtv.z });
                }
            }
        }
    }

    if(boxColliderPool && sphereColliderPool){
        auto& boxColliderList = boxColliderPool->GetList();
        auto& sphereColliderList = sphereColliderPool->GetList();

        // BoxColliderとSphereColliderの当たり判定
        for (int i = 0; i < boxColliderList.size(); i++) {
            BoxColliderComponent* colliderA = &boxColliderList[i];
            TransformComponent* transformA = transformPool->GetByGameObjectID(colliderA->GetOwner()->GetID());

            if (colliderA == nullptr || transformA == nullptr) continue;
            if (!colliderA->GetEnable() || !transformA->GetEnable()) continue;

            for (int j = 0; j < sphereColliderList.size(); j++) {
                SphereColliderComponent* colliderB = &sphereColliderList[j];
                TransformComponent* transformB = transformPool->GetByGameObjectID(colliderB->GetOwner()->GetID());

                if (colliderB == nullptr || transformB == nullptr) continue;
                if (!colliderB->GetEnable() || !transformB->GetEnable()) continue;

                // レイヤーマスクによる当たり判定スキップ
                if (!COLLISION_MATRIX[(int)colliderA->GetLayer()][(int)colliderB->GetLayer()]) continue;

                CollisionResult result = CheckBoxToSphere(
                    transformA, colliderA,
                    transformB, colliderB);

                // 衝突している場合
                if (result.isCollision) {
                    colliderA->RegisterCollisionData(colliderB, result.mtv);
                    colliderB->RegisterCollisionData(colliderA, {
                        -result.mtv.x,
                        -result.mtv.y,
                        -result.mtv.z });
                }
            }
        }
    }
#pragma endregion

    // デバッグ用コライダー描画
    DrawDebugCollider(pScene);
}

// デバッグ用コライダー描画
void CollisionPass::DrawDebugCollider(IScene* pScene)
{
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* boxColliderPool = pScene->GetComponentPool<BoxColliderComponent>();
    auto* sphereColliderPool = pScene->GetComponentPool<SphereColliderComponent>();
    if(transformPool == nullptr)return;

    const DirectX::XMFLOAT4 debugColor = { 0.0f,1.0f,0.0f,1.0f };

    if(boxColliderPool){
        auto& boxColliderList = boxColliderPool->GetList();
        // BoxColliderのデバッグ描画
        for(BoxColliderComponent& c : boxColliderList) {
            TransformComponent* t = transformPool->GetByGameObjectID(c.GetOwner()->GetID());

            // Nullチェック・Enableチェック
            if (t == nullptr) continue;
            if (!c.GetEnable() || !t->GetEnable()) continue;

            // 頂点座標を算出
            XMFLOAT3 verts[8] = {
                { -c.GetScale().x * 0.5f, -c.GetScale().y * 0.5f, -c.GetScale().z * 0.5f },
                {  c.GetScale().x * 0.5f, -c.GetScale().y * 0.5f, -c.GetScale().z * 0.5f },
                {  c.GetScale().x * 0.5f,  c.GetScale().y * 0.5f, -c.GetScale().z * 0.5f },
                { -c.GetScale().x * 0.5f,  c.GetScale().y * 0.5f, -c.GetScale().z * 0.5f },
                { -c.GetScale().x * 0.5f, -c.GetScale().y * 0.5f,  c.GetScale().z * 0.5f },
                {  c.GetScale().x * 0.5f, -c.GetScale().y * 0.5f,  c.GetScale().z * 0.5f },
                {  c.GetScale().x * 0.5f,  c.GetScale().y * 0.5f,  c.GetScale().z * 0.5f },
                { -c.GetScale().x * 0.5f,  c.GetScale().y * 0.5f,  c.GetScale().z * 0.5f },
            };

            // 回転させる
            for(int i = 0; i < 8; i++) {
                verts[i] = MiMath::RotateVector(t->GetRotation(), verts[i]);
                verts[i].x += t->GetPosition().x + c.GetCenter().x;
                verts[i].y += t->GetPosition().y + c.GetCenter().y;
                verts[i].z += t->GetPosition().z + c.GetCenter().z;
            }

            DebugRenderer_DrawLine(verts[0], verts[1], debugColor);
            DebugRenderer_DrawLine(verts[1], verts[2], debugColor);
            DebugRenderer_DrawLine(verts[2], verts[3], debugColor);
            DebugRenderer_DrawLine(verts[3], verts[0], debugColor);
            DebugRenderer_DrawLine(verts[4], verts[5], debugColor);
            DebugRenderer_DrawLine(verts[5], verts[6], debugColor);
            DebugRenderer_DrawLine(verts[6], verts[7], debugColor);
            DebugRenderer_DrawLine(verts[7], verts[4], debugColor);
            DebugRenderer_DrawLine(verts[0], verts[4], debugColor);
            DebugRenderer_DrawLine(verts[1], verts[5], debugColor);
            DebugRenderer_DrawLine(verts[2], verts[6], debugColor);
            DebugRenderer_DrawLine(verts[3], verts[7], debugColor);
        }
    }

    if(sphereColliderPool){
        auto& sphereColliderList = sphereColliderPool->GetList();
        //-----------------------------------------
        // SphereColliderのデバッグ描画
        //-----------------------------------------
        for(SphereColliderComponent& c : sphereColliderList) {
            TransformComponent* t = transformPool->GetByGameObjectID(c.GetOwner()->GetID());

            // Nullチェック・Enableチェック
            if (t == nullptr) continue;
            if (!c.GetEnable() || !t->GetEnable()) continue;

            // 円の分割数
            const int circleSegment = 16;
            const float step = DirectX::XM_2PI / circleSegment;

            const float radius = c.GetRadius();
            const XMFLOAT3 center = {
                t->GetPosition().x + c.GetCenter().x,
                t->GetPosition().y + c.GetCenter().y,
                t->GetPosition().z + c.GetCenter().z
            };
            const XMFLOAT3 rotation = t->GetEulerRotation();

            // 円の描画
            for (int i = 0; i < circleSegment; i++) {
                float theta1 = (float)i * step;
                float theta2 = (float)(i + 1) * step;

                XMFLOAT3 p1[3];
                XMFLOAT3 p2[3];

                // XY平面
                p1[0] = { radius * cosf(theta1), radius * sinf(theta1), 0.0f };
                p2[0] = { radius * cosf(theta2), radius * sinf(theta2), 0.0f };

                // YZ平面
                p1[1] = { 0.0f, radius * cosf(theta1), radius * sinf(theta1) };
                p2[1] = { 0.0f, radius * cosf(theta2), radius * sinf(theta2) };

                // ZX平面
                p1[2] = { radius * sinf(theta1), 0.0f, radius * cosf(theta1) };
                p2[2] = { radius * sinf(theta2), 0.0f, radius * cosf(theta2) };

                // 各平面ごとに描画
                for (int j = 0; j < 3; j++) {
                    // 回転・平行移動を適用
                    p1[j] = MiMath::RotateVector(rotation, p1[j]);
                    p1[j] = { p1[j].x + center.x, p1[j].y + center.y, p1[j].z + center.z };
                    p2[j] = MiMath::RotateVector(rotation, p2[j]);
                    p2[j] = { p2[j].x + center.x, p2[j].y + center.y, p2[j].z + center.z };

                    // 描画
                    DebugRenderer_DrawLine(p1[j], p2[j], debugColor);
                }
            }
        }
    }
}

#pragma region aabb bounds calculation

// BoxColliderのAABB境界情報計算
CollisionPass::Bounds CollisionPass::ConvertToBounds(
    TransformComponent* t, BoxColliderComponent* c)
{
    Bounds bounds = {};

    using namespace DirectX;

    XMMATRIX R = XMMatrixRotationQuaternion(t->GetRotation());

    // half extents (ローカル半サイズ)
    float ex = c->GetScale().x * 0.5f;
    float ey = c->GetScale().y * 0.5f;
    float ez = c->GetScale().z * 0.5f;

    // ワールド座標系での中心座標を計算
    XMFLOAT3 pos = MiMath::RotateVector(t->GetRotation(), c->GetCenter());
    pos.x += t->GetPosition().x;
    pos.y += t->GetPosition().y;
    pos.z += t->GetPosition().z;

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
CollisionPass::Bounds CollisionPass::ConvertToBounds(TransformComponent* t, SphereColliderComponent* c)
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
#pragma endregion

// AABB同士の衝突判定
CollisionPass::CollisionResult CollisionPass::CheckAABB(Bounds a, Bounds b)
{
    CollisionResult result = {};

    // 衝突判定
    result.isCollision= (
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
    if(result.isCollision){
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

    return result;
}

#pragma region collision functions

// Box同士の衝突判定
CollisionPass::CollisionResult CollisionPass::CheckBoxToBox(TransformComponent* tA, BoxColliderComponent* cA, TransformComponent* tB, BoxColliderComponent* cB)
{
    CollisionResult result = {};
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
            return result;
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

    return result;
}

// BoxとSphereの衝突判定
CollisionPass::CollisionResult CollisionPass::CheckBoxToSphere(TransformComponent* tA, BoxColliderComponent* cA, TransformComponent* tB, SphereColliderComponent* cB)
{
    CollisionResult result = { false,{0.0f,0.0f,0.0f} };

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
        {
            // 回転の逆行列（共役クォータニオン）を使って逆回転
            XMQuaternionConjugate(tA->GetRotation())
        },
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

    return result;
}

// Sphere同士の衝突判定
CollisionPass::CollisionResult CollisionPass::CheckSphereToSphere(TransformComponent* tA, SphereColliderComponent* cA, TransformComponent* tB, SphereColliderComponent* cB)
{
    CollisionResult result = { false,{0.0f,0.0f,0.0f} };

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

    return result;
}

#pragma endregion