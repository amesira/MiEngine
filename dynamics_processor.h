//----------------------------------------------------
// dynamics_processor.h [物理演算補正プロセッサー]
// 
// ・衝突情報から位置補正を行う。
// ・PhysicsProcessor → CollisionProcessor の後に行う。
// ・３番目。
// ・物体の動き、衝突判定、補正 ←ここに当たる。
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//----------------------------------------------------
#ifndef DYNAMICS_PROCESSOR_H
#define DYNAMICS_PROCESSOR_H

#include <vector>
#include "processor.h"

class TransformComponent;
class ColliderComponent;
class RigidbodyComponent;

class DynamicsProcessor : public Processor {
private:

public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;

private:
    void    ApplyDynamics(
        TransformComponent* transform,
        ColliderComponent* collider,
        RigidbodyComponent* rigidbody,
        float deltaTime);

};


#endif