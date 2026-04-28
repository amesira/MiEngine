//===================================================
// collision_pass.cpp [当たり判定プロセッサー]
//
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "collision_pass.h"
#include "collision_utility.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"

#include "Engine/System/Device/mi_fps.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/collider_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"

#include "Utility/debug_renderer.h"
#include "Utility/mi_math.h"

using namespace DirectX;

void CollisionPass::Initialize()
{

}

void CollisionPass::Finalize()
{

}

void CollisionPass::Process(IScene* pScene)
{
    float deltaTime = FPS_GetDeltaTime();

    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* rigidbodyPool = pScene->GetComponentPool<RigidbodyComponent>();
    auto* boxColliderPool = pScene->GetComponentPool<BoxColliderComponent>();
    auto* sphereColliderPool = pScene->GetComponentPool<SphereColliderComponent>();
    if (transformPool == nullptr || (boxColliderPool == nullptr && sphereColliderPool == nullptr)) {
        return;
    }

    //----------------------------------------------------
    // CCBステップ数の更新
	//----------------------------------------------------
    if (rigidbodyPool) {
        auto colliderComponents = pScene->GetComponentsByBaseType<ColliderComponent>();
        for (ColliderComponent* collider : colliderComponents) {
            RigidbodyComponent* rb = rigidbodyPool->GetByGameObjectID(collider->GetOwner()->GetID());
            if (rb == nullptr) {
                ColliderComponent::Internal::SetCCBStep(collider, 1);
                continue;
            }
            if (!rb->GetEnable()) {
                ColliderComponent::Internal::SetCCBStep(collider, 1);
                continue;
            }

            int ccbStep = CalculateCCBStep(rb, deltaTime);
            ColliderComponent::Internal::SetCCBStep(collider, ccbStep);
        }
    }

    //----------------------------------------------------
    // BoxColliderの衝突情報の更新と当たり判定
	//----------------------------------------------------
    if(boxColliderPool){
        auto& boxColliderList = boxColliderPool->GetList();

        // 衝突情報の更新
        for (BoxColliderComponent& c : boxColliderList) {
            ColliderComponent::Internal::UpdateCollisionData(&c);
        }

        // 当たり判定処理
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
                if (CollisionUtility::IsIgnoreLayerPair((int)colliderA->GetLayer(), (int)colliderB->GetLayer())) continue;

                // 詳細な衝突判定
                CollisionUtility::CollisionResult outResult = { false, {0.0f, 0.0f, 0.0f} };
                CollisionUtility::CheckOBB(outResult, transformA, colliderA,transformB, colliderB);

                // 衝突している場合
                if (outResult.isCollision) {
                    // 衝突情報の登録
                    auto slotA = ColliderComponent::Internal::RegisterCollisionData(colliderA, colliderB);
                    auto slotB = ColliderComponent::Internal::RegisterCollisionData(colliderB, colliderA);

                    // Correctionの作成
                    float correctionRateA = 0.0f;
                    float correctionRateB = 0.0f;

                    RigidbodyComponent* rbA = rigidbodyPool->GetByGameObjectID(colliderA->GetOwner()->GetID());
                    RigidbodyComponent* rbB = rigidbodyPool->GetByGameObjectID(colliderB->GetOwner()->GetID());

                    correctionRateA = CreateCorrectionRate(colliderA, rbA);
                    correctionRateB = CreateCorrectionRate(colliderB, rbB);

                    float totalRate = correctionRateA + correctionRateB;
                    if (totalRate > 0.0f) {
                        correctionRateA /= totalRate;
                        correctionRateB /= totalRate;
                    }

                    if (slotA) {
                        slotA->m_mtv = outResult.mtv;
                        slotA->m_correction = MiMath::Multiply(slotA->m_mtv, correctionRateA);
                    }
                    if (slotB) {
                        slotB->m_mtv = MiMath::Multiply(outResult.mtv, -1.0f);
                        slotB->m_correction = MiMath::Multiply(slotB->m_mtv, correctionRateB);
                    }
                }
            }
        }
    }

    if(sphereColliderPool){
        auto& sphereColliderList = sphereColliderPool->GetList();
        for (SphereColliderComponent& c : sphereColliderList) {
            ColliderComponent::Internal::UpdateCollisionData(&c);
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
                if (CollisionUtility::IsIgnoreLayerPair((int)colliderA->GetLayer(), (int)colliderB->GetLayer())) continue;

                // 詳細な衝突判定
                CollisionUtility::CollisionResult outResult = { false, {0.0f, 0.0f, 0.0f} };
                CollisionUtility::CheckSphere(outResult, transformA, colliderA, transformB, colliderB);

                // 衝突している場合
                if (outResult.isCollision) {
                    // 衝突情報の登録
                    auto slotA = ColliderComponent::Internal::RegisterCollisionData(colliderA, colliderB);
                    auto slotB = ColliderComponent::Internal::RegisterCollisionData(colliderB, colliderA);

                    // Correctionの作成
                    float correctionRateA = 0.0f;
                    float correctionRateB = 0.0f;

                    RigidbodyComponent* rbA = rigidbodyPool->GetByGameObjectID(colliderA->GetOwner()->GetID());
                    RigidbodyComponent* rbB = rigidbodyPool->GetByGameObjectID(colliderB->GetOwner()->GetID());

                    correctionRateA = CreateCorrectionRate(colliderA, rbA);
                    correctionRateB = CreateCorrectionRate(colliderB, rbB);

                    float totalRate = correctionRateA + correctionRateB;
                    if (totalRate > 0.0f) {
                        correctionRateA /= totalRate;
                        correctionRateB /= totalRate;
                    }

                    if (slotA) {
                        slotA->m_mtv = outResult.mtv;
                        slotA->m_correction = MiMath::Multiply(slotA->m_mtv, correctionRateA);
                    }
                    if (slotB) {
                        slotB->m_mtv = MiMath::Multiply(outResult.mtv, -1.0f);
                        slotB->m_correction = MiMath::Multiply(slotB->m_mtv, correctionRateB);
                    }
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
                if (CollisionUtility::IsIgnoreLayerPair((int)colliderA->GetLayer(), (int)colliderB->GetLayer())) continue;

                CollisionUtility::CollisionResult outResult = { false, {0.0f, 0.0f, 0.0f} };
                CollisionUtility::CheckOBBSphere(outResult, transformA, colliderA, transformB, colliderB);

                // 衝突している場合
                if (outResult.isCollision) {
                    // 衝突情報の登録
                    auto slotA = ColliderComponent::Internal::RegisterCollisionData(colliderA, colliderB);
                    auto slotB = ColliderComponent::Internal::RegisterCollisionData(colliderB, colliderA);

                    // Correctionの作成
                    float correctionRateA = 0.0f;
                    float correctionRateB = 0.0f;

                    RigidbodyComponent* rbA = rigidbodyPool->GetByGameObjectID(colliderA->GetOwner()->GetID());
                    RigidbodyComponent* rbB = rigidbodyPool->GetByGameObjectID(colliderB->GetOwner()->GetID());

                    correctionRateA = CreateCorrectionRate(colliderA, rbA);
                    correctionRateB = CreateCorrectionRate(colliderB, rbB);

                    float totalRate = correctionRateA + correctionRateB;
                    if (totalRate > 0.0f) {
                        correctionRateA /= totalRate;
                        correctionRateB /= totalRate;
                    }

                    if (slotA) {
                        slotA->m_mtv = outResult.mtv;
                        slotA->m_correction = MiMath::Multiply(slotA->m_mtv, correctionRateA);
                    }
                    if (slotB) {
                        slotB->m_mtv = MiMath::Multiply(outResult.mtv, -1.0f);
                        slotB->m_correction = MiMath::Multiply(slotB->m_mtv, correctionRateB);
                    }
                }
            }
        }
    }

#pragma endregion
}

// デバッグ用コライダー描画
void CollisionPass::CollectDebugDraw(IScene* pScene)
{
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* boxColliderPool = pScene->GetComponentPool<BoxColliderComponent>();
    auto* sphereColliderPool = pScene->GetComponentPool<SphereColliderComponent>();

    if (boxColliderPool) {
        auto& boxColliderList = boxColliderPool->GetList();
        for (BoxColliderComponent& c : boxColliderList) {
            TransformComponent* transform = transformPool->GetByGameObjectID(c.GetOwner()->GetID());
            if (transform == nullptr) continue;
            if (!c.GetEnable() || !transform->GetEnable()) continue;
            DrawDebug_ColliderLine(transform, &c);
        }
    }

    if (sphereColliderPool) {
        auto& sphereColliderList = sphereColliderPool->GetList();
        for (SphereColliderComponent& c : sphereColliderList) {
            TransformComponent* transform = transformPool->GetByGameObjectID(c.GetOwner()->GetID());
            if (transform == nullptr) continue;
            if (!c.GetEnable() || !transform->GetEnable()) continue;
            DrawDebug_ColliderLine(transform, &c);
        }
    }
}


// CCBステップ数の計算
int CollisionPass::CalculateCCBStep(RigidbodyComponent* rb, float deltaTime)
{
    XMFLOAT3 vel = rb->GetVelocity();
    float velocityMag = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;

    if (velocityMag > VELOCITY_CCB_THRESHOLD) {
        return static_cast<int>((velocityMag - VELOCITY_CCB_THRESHOLD) / VELOCITY_CCB_RANGE) + 1;
    }

    return 1;
}

// CorrectionRateの作成
float CollisionPass::CreateCorrectionRate(ColliderComponent* col, RigidbodyComponent* rb)
{
    // 補正値を作成しない場合
    if (!col->GetCreateCorrection()) {
        return 0.0f;
    }
    else if (rb == nullptr) {
        return 0.0f;
    }
    else {
        if (!rb->GetEnable() || rb->GetIsKinematic()) {
            return 0.0f;
        }
    }

    // 補正値を作成する場合、質量比に応じて分配するため質量をCorrectionRateとして返す
    return rb->GetMass();
}

#pragma region デバッグ用コライダー描画

// デバッグ用コライダー描画（BoxCollider）
void CollisionPass::DrawDebug_ColliderLine(TransformComponent* transform, BoxColliderComponent* boxCollider)
{
    const DirectX::XMFLOAT4 debugColor = { 0.0f,1.0f,0.0f,1.0f };

    TransformComponent* t = transform;
    BoxColliderComponent* c = boxCollider;
    if (t == nullptr || c == nullptr) return;
    if (!t->GetEnable() || !c->GetEnable()) return;

    // 頂点座標を算出
    XMFLOAT3 halfScale = MiMath::Multiply(c->GetScale(), 0.5f);
    XMFLOAT3 verts[8] = {
        { -halfScale.x, -halfScale.y, -halfScale.z },
        {  halfScale.x, -halfScale.y, -halfScale.z },
        {  halfScale.x,  halfScale.y, -halfScale.z },
        { -halfScale.x,  halfScale.y, -halfScale.z },
        { -halfScale.x, -halfScale.y,  halfScale.z },
        {  halfScale.x, -halfScale.y,  halfScale.z },
        {  halfScale.x,  halfScale.y,  halfScale.z },
        { -halfScale.x,  halfScale.y,  halfScale.z },
    };

    // 回転させる
    for (int i = 0; i < 8; i++) {
        verts[i] = MiMath::RotateVector(t->GetRotation(), verts[i]);
        verts[i].x += t->GetPosition().x + c->GetCenter().x;
        verts[i].y += t->GetPosition().y + c->GetCenter().y;
        verts[i].z += t->GetPosition().z + c->GetCenter().z;
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

// デバッグ用コライダー描画（SphereCollider）
void CollisionPass::DrawDebug_ColliderLine(TransformComponent* transform, SphereColliderComponent* sphereCollider)
{
    const DirectX::XMFLOAT4 debugColor = { 0.0f,1.0f,0.0f,1.0f };

    TransformComponent* t = transform;
    SphereColliderComponent* c = sphereCollider;
    if (t == nullptr || c == nullptr) return;
    if (!t->GetEnable() || !c->GetEnable()) return;

    // 円の分割数
    const int circleSegment = 16;
    const float step = DirectX::XM_2PI / circleSegment;

    const float radius = c->GetRadius();
    const XMFLOAT3 center = {
        t->GetPosition().x + c->GetCenter().x,
        t->GetPosition().y + c->GetCenter().y,
        t->GetPosition().z + c->GetCenter().z
    };
    const XMFLOAT3 rotation = t->GetEulerAngle();

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
#pragma endregion
