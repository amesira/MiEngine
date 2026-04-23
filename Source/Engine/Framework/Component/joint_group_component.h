//---------------------------------------------------
// joint_group_component.h [ジョイントグループコンポーネント]
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//---------------------------------------------------
#ifndef JOINT_GROUP_COMPONENT_H
#define JOINT_GROUP_COMPONENT_H
#include "Engine/Core/component.h"

class JointGroupComponent : public Component {
private:
    float   m_stiffness = 0.9f;     // 剛性係数
    float   m_kDamping = 0.03f;     // 減衰係数

    int     m_resolveIterations = 2;// 反復解決の回数

public:
    // 剛性係数を設定・取得
    void SetStiffness(float stiffness) { m_stiffness = stiffness; }
    float GetStiffness() const { return m_stiffness; }
    // 減衰係数を設定・取得
    void SetDamping(float damping) { m_kDamping = damping; }
    float GetDamping() const { return m_kDamping; }

    // 反復解決の回数を設定・取得
    void SetResolveIterations(int iterations) { m_resolveIterations = iterations; }
    int GetResolveIterations() const { return m_resolveIterations; }

};

#endif // JOINT_GROUP_COMPONENT_H