//----------------------------------------------------
// physics_processor.h [物理演算制御プロセッサー]
// 
// ・重力、摩擦などをvelocity（速度）に設定し、位置への適用を行う
// ・物理演算群の1番目にあたる。
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//----------------------------------------------------
#ifndef PHYSICS_PROCESSOR_H
#define PHYSICS_PROCESSOR_H
#include "processor.h"

#include "integrate_pass.h"
#include "collision_pass.h"
#include "resolve_pass.h"
#include "constraint_pass.h"

class PhysicsProcessor : public Processor {
private:
    IntegratePass  m_integratePass;
    CollisionPass  m_collisionPass;
    ConstraintPass m_constraintPass;
    ResolvePass    m_resolvePass;

public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;

};


#endif