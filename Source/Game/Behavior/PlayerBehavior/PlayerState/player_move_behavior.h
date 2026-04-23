//---------------------------------------------------
// player_move_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//---------------------------------------------------
#ifndef PLAYER_MOVE_BEHAVIOR_H
#define PLAYER_MOVE_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "Game/Behavior/PlayerBehavior/player_context.h"

class TransformComponent;
class RigidbodyComponent;

class PlayerMoveBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;

    float   m_moveSpeed = 17.0f;
    float   m_jumpForce = 10.0f;

    float   m_currentAngleY = 0.0f;

public:
    PlayerMoveBehavior() = default;
    ~PlayerMoveBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // 移動更新処理
    void UpdateMove(const PlayerContext& context, float deltaTime);
    // 回転更新処理
    void UpdateRotation(const PlayerContext& context, float deltaTime);

};

#endif // PLAYER_MOVE_BEHAVIOR_H