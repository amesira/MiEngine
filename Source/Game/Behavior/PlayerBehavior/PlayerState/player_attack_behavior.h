//---------------------------------------------------
// player_attack_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//---------------------------------------------------
#ifndef PLAYER_ATTACK_BEHAVIOR_H
#define PLAYER_ATTACK_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "Game/Behavior/PlayerBehavior/player_context.h"

class PlayerAttackBehavior : public BehaviorComponent {
private:
    // ホールドバッファ関連
    float m_holdBufferTimer = 0.0f; // ホールドバッファの経過時間
    float m_maxHoldBufferTime = 0.2f; // ホールドバッファの最大時間

    // 攻撃のチャージ時間
    float m_chargeTimer = 0.0f;
    float m_maxChargeTime = 2.0f; // 最大チャージ時間

public:
    PlayerAttackBehavior() {}
    ~PlayerAttackBehavior() {}

    void Start() override;
    void Update() override;

    void DrawComponentInspector() override;
    
    // ホールドバッファ開始処理
    void StartAttackHoldBuffer(PlayerContext& context);
    // ホールドバッファ更新処理
    void UpdateAttackHoldBuffer(PlayerContext& context, float deltaTime, float unscaledDeltaTime);

    // ホールドバッファ終了
    bool IsFinishedHoldBuffer() const { return m_holdBufferTimer > m_maxHoldBufferTime; }

    // エイム開始処理
    void StartAim(PlayerContext& context);
    // エイム更新処理
    void UpdateAim(PlayerContext& context, float deltaTime, float unscaledDeltaTime);
    // エイム終了処理
    void EndAim(PlayerContext& context);

    void SingleAttack(PlayerContext& context);
    void ChargeAttack(PlayerContext& context);

};

#endif // PLAYER_ATTACK_BEHAVIOR_H
