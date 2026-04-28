//----------------------------------------------------
// collision_pass.h [当たり判定パス]
// 
// ・当たり判定を取る。
// ・物理処理群の２番目。
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//----------------------------------------------------
#ifndef COLLISION_PASS_H
#define COLLISION_PASS_H
#include "Engine/Core/pass.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

class TransformComponent;
class ColliderComponent;
class RigidbodyComponent;

// コライダーの種類
class BoxColliderComponent;
class SphereColliderComponent;

class CollisionPass : public Pass {
public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;
    void    CollectDebugDraw(IScene* pScene);

private:
    // 連続衝突判定の速度閾値
    static constexpr float  VELOCITY_CCB_THRESHOLD = 200.0f;
    // 連続衝突判定の速度範囲
    static constexpr float  VELOCITY_CCB_RANGE = 70.0f;

    //---------------------------------------------------
    // 物理的な解決のための処理群
    //---------------------------------------------------
    // CCBステップ数の計算
    int     CalculateCCBStep(RigidbodyComponent* rb, float deltaTime);
    // Correctionの作成
    float   CreateCorrectionRate(ColliderComponent* col, RigidbodyComponent* rb);

    //----------------------------------------------------
    // デバッグ用コライダー描画
    //----------------------------------------------------
    void    DrawDebug_ColliderLine(TransformComponent* transform, BoxColliderComponent* boxCollider);
    void    DrawDebug_ColliderLine(TransformComponent* transform, SphereColliderComponent* sphereCollider);

};


#endif