//===================================================
// player_attack_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//===================================================
#include "player_attack_behavior.h"
#include "Engine/Core/game_object.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/camera_component.h"

#include "Game/Behavior/PlayerBehavior/player_behavior.h"

#include "Utility/mi_math.h"

#include "Game/Factory/projectile_factory.h"

void PlayerAttackBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();

    IScene* scene = GetOwner()->GetScene();
    if (scene) {
        GameObject* mainCameraObj = scene->GetGameObjectByName("MainCamera");
        if (mainCameraObj) {
            m_mainCameraTransform = mainCameraObj->GetComponent<TransformComponent>();
            m_mainCamera = mainCameraObj->GetComponent<CameraComponent>();
        }
    }
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
    // === 弾を生成 ===
    ProjectileFactory::BulletCreateDesc bulletDesc;
    bulletDesc.position = m_transform->GetPosition();
    bulletDesc.position = MiMath::Add(bulletDesc.position, MiMath::Multiply(m_transform->GetRight(), 0.5f));
    bulletDesc.radius = 0.25f;

    // 画面中心からワールド空間へのレイを計算して、弾の飛ぶ方向を決定する
    XMFLOAT3 bulletEnd = MiMath::Add(m_mainCameraTransform->GetPosition(), MiMath::Multiply(m_mainCamera->GetForward(), 30.0f));
    bulletEnd.y = bulletDesc.position.y;
    XMFLOAT3 bulletDir = MiMath::Normalize(MiMath::Subtract(bulletEnd, bulletDesc.position));
    bulletDesc.velocity = MiMath::Multiply(bulletDir, 20.0f);

    ProjectileFactory::CreateBullet(GetOwner()->GetScene(), bulletDesc);

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
    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::ChargeStart);
    }
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
    // === 弾を生成 ===
    ProjectileFactory::BulletCreateDesc bulletDesc;
    bulletDesc.position = m_transform->GetPosition();
    bulletDesc.position = MiMath::Add(bulletDesc.position, MiMath::Multiply(m_transform->GetRight(), 0.5f));
    bulletDesc.radius = 0.25f + (m_chargeTimer / m_maxChargeTime) * 0.75f; // チャージ時間に応じて弾のサイズを変化させる

    // 画面中心からワールド空間へのレイを計算して、弾の飛ぶ方向を決定する
    XMFLOAT3 bulletEnd = MiMath::Add(m_mainCameraTransform->GetPosition(), MiMath::Multiply(m_mainCamera->GetForward(), 30.0f));
    bulletEnd.y = bulletDesc.position.y;
    XMFLOAT3 bulletDir = MiMath::Normalize(MiMath::Subtract(bulletEnd, bulletDesc.position));
    bulletDesc.velocity = MiMath::Multiply(bulletDir, 20.0f);
    
    ProjectileFactory::CreateBullet(GetOwner()->GetScene(), bulletDesc);

    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::ChargeAttack);
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AttackEnd);
    }
}
