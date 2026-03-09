//----------------------------------------------------
// resolve_pass.h [物理演算補正パス]
// 
// ・衝突情報から位置補正を行う。
// ・３番目。
// ・物体の動き、衝突判定、補正 ←ここに当たる。
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//----------------------------------------------------
#ifndef RESOLVE_PASS_H
#define RESOLVE_PASS_H
#include "pass.h"

class TransformComponent;
class ColliderComponent;
class RigidbodyComponent;

class ResolvePass : public Pass {
public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;

private:
    // 衝突情報から位置補正を行う
    void    ApplyResolve(
        TransformComponent* transform,
        ColliderComponent* collider,
        RigidbodyComponent* rigidbody,
        float deltaTime);

};


#endif