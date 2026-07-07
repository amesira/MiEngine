//---------------------------------------------------
// missile_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/07/07
//---------------------------------------------------
#ifndef MISSILE_BEHAVIOR_H
#define MISSILE_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include <DirectXMath.h>
#include "Engine/Framework/Processor/PhysicsPass/Collision/collision_utility.h"

class TransformComponent;
class HitStopBehavior;
class BlinkerBehavior;

class MissileBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;

    // ミサイルの物理特性
    float m_radius = 0.25f;

    bool m_isExpired = false;       // 弾が寿命切れかどうか
    bool m_hasHit = false;          // 何かにヒットしたかどうか
    RaycastHit m_lastHit = {};      // 最後にヒットした情報

    // 軌道計算用パラメータ（ベジエ曲線）
    DirectX::XMFLOAT3 m_controlPoint[4];  // [0]が開始、[3]が目標地点

    float m_timer;
    float m_duration;
    int m_layerMask = -1;

    // === 演出関連 ===
    HitStopBehavior* m_hitStopBehavior = nullptr;
    BlinkerBehavior* m_blinkerBehavior = nullptr;

public:
    MissileBehavior();
    ~MissileBehavior() {};

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // ミサイル弾の初期化
    void Initialize(
        const DirectX::XMFLOAT3& startPosition,
        const DirectX::XMFLOAT3& controlPoint1,
        const DirectX::XMFLOAT3& controlPoint2,
        const DirectX::XMFLOAT3& targetPosition,
        float duration,
        float radius,
        int layerMask);

private:
    // ミサイルの軌道計算（t（durationからの割合）時点）
    DirectX::XMFLOAT3 EvaluateMissileTrajectory(
        const DirectX::XMFLOAT3* controlPoints, int pointCount, float t);

    // ミサイルの消滅処理
    void Vanish(bool isHitStop = false);

};


#endif // MISSILE_BEHAVIOR_H
