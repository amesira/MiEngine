//----------------------------------------------------
// constraint_pass.h [拘束解決パス]
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//----------------------------------------------------
#ifndef CONSTRAINT_PASS_H
#define CONSTRAINT_PASS_H
#include "pass.h"

#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

class RigidbodyComponent;
class JointComponent;
class JointGroupComponent;

class ConstraintPass : public Pass {
public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;

private:
    // ジョイントのグループ構造体
    struct MassPoint {
        unsigned int groupID = 0;
        unsigned int id = 0;

        XMFLOAT3 position = { 0.0f,0.0f,0.0f };
        XMFLOAT3 velocity = { 0.0f,0.0f,0.0f };
        float mass = 1.0f;
    };
    std::vector<MassPoint> m_jointMassPoints;

    // VelocityDampingの適用
    void    ApplyVelocityDamping(
        JointGroupComponent* group,
        std::vector<MassPoint*>& massPointsInGroup);

};

#endif // CONSTRAINT_PASS_H