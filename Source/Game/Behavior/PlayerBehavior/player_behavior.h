//---------------------------------------------------
// player_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef PLAYER_BEHAVIOR_H
#define PLAYER_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"

#include "Engine/Device/direct3d.h"
using namespace DirectX;

class RigidbodyComponent;
class SpriteRendererComponent;
class SpriteAnimationComponent;

#include "player_context.h"
class PlayerStateMachineBehavior;
class PlayerCombatMachineBehavior;

class TransformComponent;
class CameraComponent;

class ParticleSystemComponent;
class LightComponent;

#include "Engine/Core/GamePlay/sequence_task.h"

class HitStopBehavior;
class AfterimageGeneratorBehavior;

class PlayerBehavior : public BehaviorComponent {
private:
    RigidbodyComponent* m_rigidbody = nullptr;
    SpriteRendererComponent* m_spriteRenderer = nullptr;
    SpriteAnimationComponent* m_spriteAnimation = nullptr;

    // プレイヤーコンテキスト
    PlayerContext m_context;

    // プレイヤー制御マシーン
    PlayerStateMachineBehavior* m_stateMachine = nullptr;
    PlayerCombatMachineBehavior* m_combatMachine = nullptr;

    // メインカメラの参照
    TransformComponent* m_mainCameraTransform = nullptr;
    CameraComponent* m_mainCamera = nullptr;

    // === プレイヤーエフェクト ===
    bool m_lockMovement = false; // プレイヤーの移動をロックするかどうか

    HitStopBehavior* m_hitStopBehavior = nullptr;
    AfterimageGeneratorBehavior* m_afterimageGenerator = nullptr;

    ParticleSystemComponent* m_chargeEffect = nullptr;
    LightComponent* m_chargeLight = nullptr;

    // チャージライトの変化を管理するタスク
    class ChangeChargeLightTask : public SequenceTask {
    public:
        LightComponent* m_chargeLight = nullptr;

        XMFLOAT4 m_defaultColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT4 m_startColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT4 m_targetColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT4 m_endColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        float m_defaultIntensity = 0.0f;
        float m_startIntensity = 0.0f;
        float m_targetIntensity = 0.0f;
        float m_endIntensity = 0.0f;

        float m_duration = 0.0f;
        float m_holdDuration = 0.0f;

        void Start() override;
        void Update(float deltaTime) override;
    };

    ChangeChargeLightTask m_changeChargeLightTask;

public:
    ~PlayerBehavior() = default;
    void    Start() override;
    void    Update() override;
    void    DrawComponentInspector() override;

    void PlayPlayerEffect(PlayerEffectType type);

    // チャージエフェクトセットアップ
    void SetupChargeEffect(ParticleSystemComponent* chargeEffect) { m_chargeEffect = chargeEffect; }
    // チャージライトセットアップ
    void SetupChargeLight(LightComponent* chargeLight);

private:
    // プレイヤーの入力処理
    PlayerInput UpdateInput();
    // プレイヤーのアニメーション制御
    void UpdateAnimation(PlayerState state, PlayerCombatState combatState);

    void ChangeChargeLight(const XMFLOAT4& color, float intensity, float duration);
    void ChangeChargeLightTemporary(const XMFLOAT4& color, float intensity, float duration, float holdDuration);
    void ResetChargeLight(float duration);

};

#endif
