//===================================================
// player_attack_behavior.cpp
// 
// Author・Miu Kitamura
// Date  ・・026/03/25
//===================================================
#include "player_attack_behavior.h"

#include "Game/Behavior/PlayerBehavior/player_behavior.h"

void PlayerAttackBehavior::Start()
{

}

void PlayerAttackBehavior::Update()
{

}

void PlayerAttackBehavior::DrawComponentInspector()
{

}

// ------------------------------- public

// === Aim Hold Buffer ===
// エイム移行バッファ開始処理
void PlayerAttackBehavior::StartAimHoldBuffer(PlayerContext& context)
{
    m_aimHoldBufferTimer = 0.0f;

    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AimHoldStart);
    }
}

// エイム移行バッファ更新処理
void PlayerAttackBehavior::UpdateAimHoldBuffer(PlayerContext& context, float deltaTime, float unscaledDeltaTime)
{
    m_aimHoldBufferTimer += unscaledDeltaTime;
}

// === Aim ===
// エイム開始処理
void PlayerAttackBehavior::StartAim(PlayerContext& context)
{
    m_chargeTimer = 0.0f;

    // エイム開始エフェクトの再生
    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AimStart);
    }
}

// エイム更新処理
void PlayerAttackBehavior::UpdateAim(PlayerContext& context, float deltaTime, float unscaledDeltaTime)
{
    m_chargeTimer += unscaledDeltaTime;
    if (m_chargeTimer > m_maxChargeTime) {
        m_chargeTimer = m_maxChargeTime;
    }
}

// エイム終了処理
void PlayerAttackBehavior::EndAim(PlayerContext& context)
{
    // エイム終了エフェクトの再生
    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AimEnd);
    }
}

// === Attack Hold Buffer ===
// 攻撃移行バッファ開始処理
void PlayerAttackBehavior::StartAttackHoldBuffer(PlayerContext& context)
{
    m_attackHoldBufferTimer = 0.0f;
}

// 攻撃移行バッファ更新処理
void PlayerAttackBehavior::UpdateAttackHoldBuffer(PlayerContext& context, float deltaTime, float unscaledDeltaTime)
{
    m_attackHoldBufferTimer += unscaledDeltaTime;
}

// === Single Attack ===
// 単発攻撃処理
void PlayerAttackBehavior::SingleAttack(PlayerContext& context)
{
    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::SingleAttack);
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AttackEnd);
    }
}

// === Charge Attack ===
// チャージ開始処理
void PlayerAttackBehavior::StartCharge(PlayerContext& context)
{
    m_chargeTimer = 0.0f;
}

// チャージ更新処理
void PlayerAttackBehavior::UpdateCharge(PlayerContext& context, float deltaTime, float unscaledDeltaTime)
{
    m_chargeTimer += unscaledDeltaTime;
    if (m_chargeTimer > m_maxChargeTime) {
        m_chargeTimer = m_maxChargeTime;
    }
}

// チャージ攻撃処理
void PlayerAttackBehavior::ChargeAttack(PlayerContext& context)
{
    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::ChargeAttack);
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AttackEnd);
    }
}
