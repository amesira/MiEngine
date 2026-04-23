//===================================================
// constraint_pass.cpp [拘束解決パス]
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//===================================================
#include "constraint_pass.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"

#include "Engine/System/Device/mi_fps.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/collider_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/joint_component.h"
#include "Engine/Framework/Component/joint_group_component.h"

#include "Utility/debug_renderer.h"
#include "Utility/mi_math.h"


void ConstraintPass::Initialize()
{
    m_jointMassPoints.reserve(1024);
}

void ConstraintPass::Finalize()
{

}

void ConstraintPass::Process(IScene* pScene)
{
    float deltaTime = FPS_GetDeltaTime();

    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    auto* rigidbodyPool = pScene->GetComponentPool<RigidbodyComponent>();
    auto* jointPool = pScene->GetComponentPool<JointComponent>();
    auto* jointGroupPool = pScene->GetComponentPool<JointGroupComponent>();
    if (transformPool == nullptr || rigidbodyPool == nullptr || jointPool == nullptr || jointGroupPool == nullptr)return;

    auto& jointList = jointPool->GetList();
    for (JointComponent& j : jointList) {
        JointComponent* joint = &j;
        RigidbodyComponent* rigidbodyA = rigidbodyPool->GetByGameObjectID(joint->GetOwner()->GetID());
        TransformComponent* transformA = transformPool->GetByGameObjectID(joint->GetOwner()->GetID());
        RigidbodyComponent* rigidbodyB = rigidbodyPool->GetByGameObjectID(joint->GetConnectedBodyID());
        TransformComponent* transformB = transformPool->GetByGameObjectID(joint->GetConnectedBodyID());

        JointGroupComponent* group = jointGroupPool->GetByGameObjectID(joint->GetGroupID());

        // コンポーネントが無効ならスキップ
        if (!transformA || !rigidbodyA || !transformB || !rigidbodyB || !group)continue;
        if (!joint->GetEnable() || !rigidbodyA->GetEnable()|| !rigidbodyB->GetEnable() || !group->GetEnable())continue;

        // 1.推定位置を計算
        XMFLOAT3 estPosA = transformA->GetPosition();
        XMFLOAT3 estPosB = transformB->GetPosition();

        //----------------------------------------------------
        for (int i = 0; i < group->GetResolveIterations(); i++) {
            // 2.拘束解決
            XMFLOAT3 correctionA, correctionB;
            {
                float massA = rigidbodyA->GetMass();
                float massB = rigidbodyB->GetMass();

                // 逆数を計算
                float w1 = (massA > 0.0f) ? 1.0f / massA : 0.0f;
                float w2 = (massB > 0.0f) ? 1.0f / massB : 0.0f;

                // 目標との差分
                float restLength = joint->GetRestLength();
                float diff = MiMath::Distance(estPosA, estPosB) - restLength;

                // 補正量を計算
                float correction = diff / (w1 + w2) * group->GetStiffness();
                XMFLOAT3 dir = MiMath::Normalize(MiMath::Subtract(estPosB, estPosA));

                // 補正量を分配
                correctionA = MiMath::Multiply(dir, correction * w1);
                correctionB = MiMath::Multiply(dir, -correction * w2);
            }

            // 3.推定位置に補正量を加算
            estPosA = MiMath::Add(estPosA, correctionA);
            estPosB = MiMath::Add(estPosB, correctionB);
        }
        //---------------------------------------------------- 反復するならこの間

        // キネマティックは位置を変えないようにする
        if (rigidbodyA->GetIsKinematic())estPosA = transformA->GetPosition();
        if (rigidbodyB->GetIsKinematic())estPosB = transformB->GetPosition();

        transformA->SetPosition(estPosA);
        transformB->SetPosition(estPosB);

        // 4.速度の更新
        {
            XMFLOAT3 newVelA = MiMath::Subtract(estPosA, transformA->GetPrevPosition());
            XMFLOAT3 newVelB = MiMath::Subtract(estPosB, transformB->GetPrevPosition());
            rigidbodyA->SetVelocity(MiMath::Multiply(newVelA, 1.0f / deltaTime));
            rigidbodyB->SetVelocity(MiMath::Multiply(newVelB, 1.0f / deltaTime));
        }
    }

    // MassPointへ情報を登録
    m_jointMassPoints.clear();
    for (JointComponent& j : jointList) {
        JointComponent* joint = &j;
        TransformComponent* transform = transformPool->GetByGameObjectID(joint->GetOwner()->GetID());
        RigidbodyComponent* rigidbody = rigidbodyPool->GetByGameObjectID(joint->GetOwner()->GetID());

        // コンポーネントが無効ならスキップ
        if (!transform || !rigidbody)continue;
        if (!joint->GetEnable() || !transform->GetEnable() || !rigidbody->GetEnable())continue;

        MassPoint massPoint;
        massPoint.groupID = joint->GetGroupID();
        massPoint.id = joint->GetOwner()->GetID();
        massPoint.position = transform->GetPosition();
        massPoint.velocity = rigidbody->GetVelocity();
        massPoint.mass = rigidbody->GetMass();
        m_jointMassPoints.push_back(massPoint);
    }

    // VelocityDampingの適用
    auto& jointGroupList = jointGroupPool->GetList();
    for (JointGroupComponent& g : jointGroupList) {
        JointGroupComponent* group = &g;

        // コンポーネントが無効ならスキップ
        if (!group->GetEnable())continue;

        // グループIDに対応するMassPointのリストを取得
        std::vector<MassPoint*> massPointsInGroup;
        for (MassPoint& mp : m_jointMassPoints) {
            if (mp.groupID == group->GetOwner()->GetID()) {
                massPointsInGroup.push_back(&mp);
            }
        }
        if (massPointsInGroup.empty())continue;

        // VelocityDampingの適用
        ApplyVelocityDamping(group, massPointsInGroup);
    }

    // VelocityDampingの結果をRigidbodyComponentに反映
    for (const MassPoint& mp : m_jointMassPoints) {
        RigidbodyComponent* rigidbody = rigidbodyPool->GetByGameObjectID(mp.id);
        if (!rigidbody)continue;
        if (!rigidbody->GetEnable())continue;

        if (rigidbody->GetIsKinematic()) {
            rigidbody->SetVelocity({ 0.0f, 0.0f, 0.0f });
        }
        else {
            rigidbody->SetVelocity(mp.velocity);
        }
    }
}

// グループ内のMassPointにVelocityDampingを適用
void ConstraintPass::ApplyVelocityDamping(
    JointGroupComponent* group, std::vector<MassPoint*>& massPointsInGroup)
{
    // 1.重心位置、重心速度を計算
    XMFLOAT3 centerPos = { 0.0f,0.0f,0.0f };
    XMFLOAT3 centerVel = { 0.0f,0.0f,0.0f };
    float totalMass = 0.0f;
    for (const MassPoint* mp : massPointsInGroup) {
        centerPos = MiMath::Add(centerPos, MiMath::Multiply(mp->position, mp->mass));
        centerVel = MiMath::Add(centerVel, MiMath::Multiply(mp->velocity, mp->mass));
        totalMass += mp->mass;
    }

    if (totalMass > 0.0f) {
        centerPos = MiMath::Multiply(centerPos, 1.0f / totalMass);
        centerVel = MiMath::Multiply(centerVel, 1.0f / totalMass);
    }

    // 2.角運動量Lを計算
    XMFLOAT3 L = { 0.0f,0.0f,0.0f };
    for (const MassPoint* mp : massPointsInGroup) {
        XMFLOAT3 r = MiMath::Subtract(mp->position, centerPos);
        XMFLOAT3 x = MiMath::Multiply(mp->velocity, mp->mass);
        
        L = MiMath::Add(L, MiMath::Cross(r, x));
    }

    // 3.慣性テンソルIを計算
    XMMATRIX I = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    for (const MassPoint* mp : massPointsInGroup) {
        XMFLOAT3 r = MiMath::Subtract(mp->position, centerPos);
        XMMATRIX R = {
            0.0f, -r.z, r.y, 0.0f,
            r.z, 0.0f, -r.x, 0.0f,
            -r.y, r.x, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        I += (R * XMMatrixTranspose(R)) * mp->mass;
    }

    // 4.角速度ωを計算
    XMFLOAT3 omega = { 0.0f,0.0f,0.0f };
    {
        XMMATRIX I_inv = XMMatrixInverse(nullptr, I);
        XMVECTOR L_vec = XMLoadFloat3(&L);
        XMVECTOR omega_vec = XMVector3Transform(L_vec, I_inv);
        XMStoreFloat3(&omega, omega_vec);
    }

    // 5.各MassPointの速度を減衰
    for (MassPoint* mp : massPointsInGroup) {
        XMFLOAT3 r = MiMath::Subtract(mp->position, centerPos);
        XMFLOAT3 vel_correction = MiMath::Cross(omega, r);
        XMFLOAT3 delta_v = MiMath::Add(centerVel, MiMath::Subtract(vel_correction, mp->velocity));

        // 減衰率を適用
        float damping = group->GetDamping();
        mp->velocity = MiMath::Add(mp->velocity, MiMath::Multiply(delta_v, damping));
    }
}
