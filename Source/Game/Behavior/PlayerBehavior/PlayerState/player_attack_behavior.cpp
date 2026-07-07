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

#include "Game/Behavior/BulletBehavior/bezier_line_preview_behavior.h"

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
    bulletDesc.lifeTime = 5.0f;
    bulletDesc.layerMask = (int)CollisionLayer::Bullet;

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

    for (int i = 0; i < MISSILE_PREVIEW_LINE_COUNT; i++) {
        if (m_missilePreviewLines[i]) {
            m_missilePreviewLines[i]->SetEnable(true);
        }
    }
}

// チャージ更新処理
void PlayerAttackBehavior::UpdateCharge(PlayerContext& context, float deltaTime, float unscaledDeltaTime)
{
    m_chargeTimer += unscaledDeltaTime;
    if (m_chargeTimer > m_maxChargeTime) {
        m_chargeTimer = m_maxChargeTime;
    }

    XMFLOAT3 startPosition = MiMath::Add(m_transform->GetPosition(), MiMath::Multiply(m_transform->GetRight(), 0.5f));
    XMFLOAT3 targetPosition = MiMath::Add(m_mainCameraTransform->GetPosition(), MiMath::Multiply(m_mainCamera->GetForward(), 40.0f));

    // ミサイルプレビューラインの更新
    for (int i = 0; i < MISSILE_PREVIEW_LINE_COUNT; i++) {
        if (m_missilePreviewLines[i]) {
            XMFLOAT3 controlPoint1, controlPoint2;
            GetMissilePoints(i, startPosition, targetPosition, controlPoint1, controlPoint2);

            m_missilePreviewLines[i]->SetControlPoints(startPosition, controlPoint1, controlPoint2, targetPosition);
        }
    }
}

// チャージ攻撃処理
void PlayerAttackBehavior::ChargeAttack(PlayerContext& context)
{
    // === ミサイル弾を生成 ===
    ProjectileFactory::MissileCreateDesc missileDesc;
    missileDesc.startPosition = MiMath::Add(m_transform->GetPosition(), MiMath::Multiply(m_transform->GetRight(), 0.5f));
    missileDesc.targetPosition = MiMath::Add(m_mainCameraTransform->GetPosition(), MiMath::Multiply(m_mainCamera->GetForward(), 40.0f));
    missileDesc.duration = 1.5f;
    missileDesc.layerMask = (int)CollisionLayer::Bullet;

    for (int i = 0; i < 5; i++) {
        XMFLOAT3 controlPoint1, controlPoint2;
        GetMissilePoints(i, missileDesc.startPosition, missileDesc.targetPosition, controlPoint1, controlPoint2);
        missileDesc.controlPoint1 = controlPoint1;
        missileDesc.controlPoint2 = controlPoint2;

        ProjectileFactory::CreateMissile(GetOwner()->GetScene(), missileDesc);
    }

    if (context.playerBehavior) {
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::ChargeAttack);
        context.playerBehavior->PlayPlayerEffect(PlayerEffectType::AttackEnd);
    }

    // ミサイルプレビューラインを非表示にする
    for (int i = 0; i < MISSILE_PREVIEW_LINE_COUNT; i++) {
        if (m_missilePreviewLines[i]) {
            m_missilePreviewLines[i]->ClearLine();
            m_missilePreviewLines[i]->SetEnable(false);
        }
    }
}

// ミサイルコントロールポイントの取得
void PlayerAttackBehavior::GetMissilePoints(int index, const XMFLOAT3& startPosition, const XMFLOAT3& targetPosition, XMFLOAT3& controlPoint1, XMFLOAT3& controlPoint2)
{
    const XMFLOAT2 controlPoint1Offset[5] = {
        { 0.0f, 0.0f },
        { 2.0f, 0.0f },
        { -2.0f, 0.0f },
        { 1.5f, 0.0f },
        { -1.5f, 0.0f }
    };

    const XMFLOAT2 controlPoint2Offset[5] = {
        { 0.0f, 5.0f },
        { 1.0f, 4.0f },
        { -1.0f, 6.0f },
        { 2.0f, 3.0f },
        { -2.0f, 7.0f}
    };

    const float power = 10.0f;

    controlPoint1 = MiMath::Add(startPosition, MiMath::Multiply(m_transform->GetForward(), 15.0f));
    controlPoint1.x += controlPoint1Offset[index].x * power;
    controlPoint1.z += controlPoint1Offset[index].x * power;

    controlPoint2 = MiMath::Add(targetPosition, MiMath::Multiply(m_mainCamera->GetForward(), 30.0f));
    controlPoint2.x += controlPoint2Offset[index].x * power;
    controlPoint2.y += controlPoint2Offset[index].y * power;
    controlPoint2.z += controlPoint2Offset[index].x * power;
}

void PlayerAttackBehavior::SetupMissilePreviewLines(
    const std::array<BezierLinePreviewBehavior*, MISSILE_PREVIEW_LINE_COUNT>& previewLines)
{
    m_missilePreviewLines = previewLines;
}
