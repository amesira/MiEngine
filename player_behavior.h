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
class AnimationComponent;

class MaterialResource;

class PlayerMoveBehavior;
class PlayerAttackBehavior;

class PlayerBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;
    ModelComponent* m_model = nullptr;
    AnimationComponent* m_animation = nullptr;

    PlayerMoveBehavior*     m_moveBehavior = nullptr;
    PlayerAttackBehavior*   m_attackBehavior = nullptr;

    // プレイヤー入力構造体
    struct PlayerInput {
        float moveX = 0.0f;         // 水平方向の移動入力（-1.0f～1.0f）
        float moveZ = 0.0f;         // 前後方向の移動入力（-1.0f～1.0f）
        bool triggerJump = false;   // ジャンプ入力
    };
    PlayerInput m_input;

    // プレイヤー移動パラメータ
    float   m_moveSpeed = 8.0f;
    float   m_jumpForce = 17.0f;

    float   m_currentAngleY = 0.0f; // プレイヤーの現在のY軸回転角度

    MaterialResource* m_myMaterial = nullptr;

public:
    ~PlayerBehavior() = default;
    void    Start() override;
    void    Update() override;
    void    DrawComponentInspector() override;

private:
    // プレイヤーの入力処理
    PlayerInput UpdateInput(const PlayerInput& currentInput);

};

#endif