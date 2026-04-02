//---------------------------------------------------
// player_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef PLAYER_BEHAVIOR_H
#define PLAYER_BEHAVIOR_H
#include "behavior_component.h"

#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class RigidbodyComponent;
class ModelComponent;

class MaterialResource;

class PlayerMoveBehavior;
class PlayerAttackBehavior;

class PlayerBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;
    ModelComponent* m_model = nullptr;

    // プレイヤーを構成する各種ビヘイビア
    PlayerMoveBehavior*     m_moveBehavior = nullptr;
    PlayerAttackBehavior*   m_attackBehavior = nullptr;

    XMFLOAT3 m_moveDirection = { 0.0f,0.0f,0.0f };

    float m_moveSpeed = 2.0f;

    MaterialResource* m_myMaterial = nullptr;

public:
    PlayerBehavior() {}
    ~PlayerBehavior() {}

    void    Start() override;
    void    Update() override;

    void    DrawComponentInspector() override;

};

#endif