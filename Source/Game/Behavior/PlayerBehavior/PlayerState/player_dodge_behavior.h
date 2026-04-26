//---------------------------------------------------
// player_dodge_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/26
//---------------------------------------------------
#ifndef PLAYER_DODGE_BEHAVIOR_H
#define PLAYER_DODGE_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "../player_context.h"

class TransformComponent;
class RigidbodyComponent;

class PlayerDodgeBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;

    // 回避中かどうか
    bool m_isDodging = false;

    // ジャスト回避かどうか
    bool m_isJustDodge = false;

    // 回避時間
    float m_dodgeDuration = 2.0f;
    float m_dodgeTimer = 0.0f;

public:
    PlayerDodgeBehavior() = default;
    ~PlayerDodgeBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // 回避状態開始処理
    void StartDodge(PlayerContext& context);
    // 回避状態更新処理
    void UpdateDodge(PlayerContext& context, float deltaTime);

    // 回避状態終了チェック
    bool IsDodgeFinished() const { return !m_isDodging; }

private:

};

#endif // PLAYER_DODGE_BEHAVIOR_H