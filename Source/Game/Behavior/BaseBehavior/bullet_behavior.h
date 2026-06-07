//---------------------------------------------------
// bullet_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/06
//---------------------------------------------------
#ifndef BULLET_BEHAVIOR_H
#define BULLET_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "Engine/Framework/Processor/PhysicsPass/Collision/collision_utility.h"

#include <functional>

class TransformComponent;
class HitStopBehavior;

class BulletBehavior : public BehaviorComponent {
public:
    // ヒットコールバックの型定義
    using HitCallback = std::function<void(const RaycastHit&)>;

private:
    TransformComponent* m_transform = nullptr;

    // 弾の物理特性
    DirectX::XMFLOAT3 m_velocity = { 0.0f, 0.0f, 0.0f };
    float m_radius = 0.25f;
    float m_lifeTime = 5.0f;
    float m_lifeTimer = 0.0f;
    int m_layerMask = -1;

    bool m_isExpired = false;       // 弾が寿命切れかどうか
    bool m_hasHit = false;          // 何かにヒットしたかどうか
    RaycastHit m_lastHit = {};      // 最後にヒットした情報
    HitCallback m_onHit = nullptr;  // ヒットコールバック関数

    // ヒットストップ
    HitStopBehavior* m_hitStopBehavior = nullptr;

public:
    BulletBehavior() = default;
    ~BulletBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // 弾の初期化
    void Initialize(const DirectX::XMFLOAT3& velocity, float radius, float lifeTime, int layerMask = -1);

    // 弾の速度の設定・取得
    void SetVelocity(const DirectX::XMFLOAT3& velocity) { m_velocity = velocity; }
    DirectX::XMFLOAT3 GetVelocity() const { return m_velocity; }

    // 弾の半径の設定・取得
    void SetRadius(float radius);
    float GetRadius() const { return m_radius; }

    // 弾の寿命の設定・取得
    void SetLifeTime(float lifeTime) { m_lifeTime = lifeTime; }
    float GetLifeTime() const { return m_lifeTime; }

    // レイヤーマスクの設定・取得
    void SetLayerMask(int layerMask) { m_layerMask = layerMask; }
    int GetLayerMask() const { return m_layerMask; }

    // ヒットコールバックの設定
    void SetOnHit(HitCallback onHit) { m_onHit = onHit; }

    // 状態の取得
    bool IsExpired() const { return m_isExpired; }
    bool HasHit() const { return m_hasHit; }
    const RaycastHit& GetLastHit() const { return m_lastHit; }

    // 弾の寿命切れ処理
    void Finalize(bool isHitStop = false);
};

#endif // BULLET_BEHAVIOR_H
