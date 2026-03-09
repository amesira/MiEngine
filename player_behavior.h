//---------------------------------------------------
// player_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef PLAYER_BEHAVIOR_H
#define PLAYER_BEHAVIOR_H
#include "behavior.h"

#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class RigidbodyComponent;

class PlayerBehavior : public Behavior {
private:
    TransformComponent* m_transform = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;
    XMFLOAT3 m_moveDirection = { 0.0f,0.0f,0.0f };

public:
    PlayerBehavior(GameObject* owner);
    ~PlayerBehavior();

    void    Update(IScene* pScene) override;

};

#endif