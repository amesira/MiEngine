//---------------------------------------------------
// bullet_behavior.h
// 
// Author・Miu Kitamura
// Date  ・・026/06/06
//---------------------------------------------------
#ifndef BULLET_BEHAVIOR_H
#define BULLET_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"
#include "Engine/Framework/Processor/PhysicsPass/Collision/collision_utility.h"

#include <functional>

class TransformComponent;

class BulletBehavior : public BehaviorComponent {
public:
    using HitCallback = std::function<void(const RaycastHit&)>;

private:
    TransformComponent* m_transform = nullptr;

    DirectX::XMFLOAT3 m_velocity = { 0.0f, 0.0f, 0.0f };
    float m_radius = 0.25f;
    float m_lifeTime = 5.0f;
    float m_lifeTimer = 0.0f;
    int m_layerMask = -1;

    bool m_isExpired = false;
    bool m_hasHit = false;
    RaycastHit m_lastHit = {};
    HitCallback m_onHit = nullptr;

public:
    BulletBehavior() = default;
    ~BulletBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    void Initialize(const DirectX::XMFLOAT3& velocity, float radius, float lifeTime, int layerMask = -1);

    void SetVelocity(const DirectX::XMFLOAT3& velocity) { m_velocity = velocity; }
    DirectX::XMFLOAT3 GetVelocity() const { return m_velocity; }

    void SetRadius(float radius);
    float GetRadius() const { return m_radius; }

    void SetLifeTime(float lifeTime) { m_lifeTime = lifeTime; }
    float GetLifeTime() const { return m_lifeTime; }

    void SetLayerMask(int layerMask) { m_layerMask = layerMask; }
    int GetLayerMask() const { return m_layerMask; }

    void SetOnHit(HitCallback onHit) { m_onHit = onHit; }

    bool IsExpired() const { return m_isExpired; }
    bool HasHit() const { return m_hasHit; }
    const RaycastHit& GetLastHit() const { return m_lastHit; }

    void Expire();
};

#endif // BULLET_BEHAVIOR_H
