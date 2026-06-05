//===================================================
// player_attack_behavior.cpp
// 
// Author・哺iu Kitamura
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

void PlayerAttackBehavior::StartAttackHoldBuffer(PlayerContext& context)
{
    m_holdBufferTimer = 0.0f;

    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AttackHoldStart);
    }
}

void PlayerAttackBehavior::UpdateAttackHoldBuffer(PlayerContext& context, float deltaTime, float unscaledDeltaTime)
{
    (void)context;
    (void)deltaTime;

    m_holdBufferTimer += unscaledDeltaTime;
}

// エイム開始処理
void PlayerAttackBehavior::StartAim(PlayerContext& context)
{
    m_chargeTimer = 0.0f;

    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AimStart);
    }
}

// エイム更新処理
void PlayerAttackBehavior::UpdateAim(PlayerContext& context, float deltaTime, float unscaledDeltaTime)
{
    (void)context;
    (void)deltaTime;

    m_chargeTimer += unscaledDeltaTime;
    if (m_chargeTimer > m_maxChargeTime) {
        m_chargeTimer = m_maxChargeTime;
    }
}

// エイム終了処理
void PlayerAttackBehavior::EndAim(PlayerContext& context)
{
    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AimEnd);
    }
}

void PlayerAttackBehavior::SingleAttack(PlayerContext& context)
{
    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::SingleAttack);
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AttackEnd);
    }
}

void PlayerAttackBehavior::ChargeAttack(PlayerContext& context)
{
    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::ChargeAttack);
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AttackEnd);
    }
}
