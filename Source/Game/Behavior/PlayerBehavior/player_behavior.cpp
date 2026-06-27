//===================================================
// player_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "player_behavior.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Utility/mi_math.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Device/keyboard.h"
#include "Engine/Device/mouse.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include <algorithm>

// コンポーネント
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/animation_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/sprite_animation_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"
#include "Engine/Framework/Component/light_component.h"

#include "Engine/Framework/Component/particle_system_component.h"

// プレイヤーを構成する各種ビヘイビアのヘッダ
#include "player_state_machine_behavior.h"
#include "player_combat_machine_behavior.h"

#include "./PlayerState/player_move_behavior.h"
#include "./PlayerState/player_attack_behavior.h"
#include "./PlayerState/player_dodge_behavior.h"

#include "Game/Behavior/BaseBehavior/hit_stop_behavior.h"
#include "Game/Behavior/BaseBehavior/afterimage_generator_behavior.h"

#include "Game/ControllerBehavior/game_controller_locator.h"
#include "Game/ControllerBehavior/game_effect_controller.h"
#include "Game/ControllerBehavior/custom_post_effect_controller.h"
#define GAME_EFFECT GameControllerLocator::GetGameEffectController()
#define CUSTOM_POST_EFFECT GameControllerLocator::GetCustomPostEffectController()

void PlayerBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;

    m_rigidbody = owner->GetComponent<RigidbodyComponent>();

    m_spriteRenderer = owner->GetComponent<SpriteRendererComponent>();
    m_spriteAnimation = owner->GetComponent<SpriteAnimationComponent>();
    
    m_stateMachine = owner->GetComponent<PlayerStateMachineBehavior>();
    m_combatMachine = owner->GetComponent<PlayerCombatMachineBehavior>();

    m_afterimageGenerator = owner->GetComponent<AfterimageGeneratorBehavior>();
    m_hitStopBehavior = owner->GetComponent<HitStopBehavior>();

    m_context.moveBehavior = owner->GetComponent<PlayerMoveBehavior>();
    m_context.attackBehavior = owner->GetComponent<PlayerAttackBehavior>();
    m_context.dodgeBehavior = owner->GetComponent<PlayerDodgeBehavior>();
    m_context.playerBehavior = this;

    IScene* scene = owner->GetScene();

    // メインカメラの参照取得
    {
        GameObject* mainCamera = scene->GetGameObjectByName("MainCamera");
        if (mainCamera) {
            m_mainCameraTransform = mainCamera->GetComponent<TransformComponent>();
            m_mainCamera = mainCamera->GetComponent<CameraComponent>();
        }
    }
}

void PlayerBehavior::Update()
{
    float deltaTime = FPS_GetDeltaTime();
    float unscaledDeltaTime = FPS_GetUnscaledDeltaTime();

    // 入力の更新
    m_context.input = UpdateInput();

    PlayerMoveRequest moveRequest;

    // 状態マシーンの更新
    if (m_stateMachine) {
        m_stateMachine->UpdateStateMachine(m_context, moveRequest, deltaTime);
    }

    // 戦闘マシーンの更新
    if (m_combatMachine) {
        m_combatMachine->UpdateCombatMachine(m_context, moveRequest, deltaTime, unscaledDeltaTime);
    }

    // 移動・回転の更新
    if (m_context.moveBehavior) {
        if (m_lockMovement) { // 強制的に移動をロックする場合
            moveRequest.canMove = false; // 移動をロック
            moveRequest.canRotate = false; // 回転もロック
        }
        m_context.moveBehavior->UpdateMove(m_context, moveRequest, deltaTime);
        m_context.moveBehavior->UpdateRotation(m_context, moveRequest, deltaTime);
    }

    // アニメーション制御
    if (!m_lockMovement) {
        UpdateAnimation(m_context.state, m_context.combatState);
    }

    // === プレイヤーエフェクトの更新 ===
    m_changeChargeLightTask.Update(unscaledDeltaTime);
}

// PlayerBehaviorのインスペクタ表示
void PlayerBehavior::DrawComponentInspector()
{
    if(InspectorViewWindow::BeginComponentSection(this, "Player Behavior")) {
        // 参照状態の表示
        ImGui::Text("StateMachine: %s", m_stateMachine ? "OK" : "None");
        ImGui::Text("CombatMachine: %s", m_combatMachine ? "OK" : "None");

        // 入力状態の表示
        ImGui::Text(
            "MoveInputCameraLocal: (%.2f, %.2f, %.2f)", 
            m_context.input.moveInputCameraLocal.x,
            m_context.input.moveInputCameraLocal.y,
            m_context.input.moveInputCameraLocal.z);
    }

    InspectorViewWindow::EndComponentSection();
}

// プレイヤーエフェクトの再生
void PlayerBehavior::SetupChargeLight(LightComponent* chargeLight)
{
    m_chargeLight = chargeLight;
    m_changeChargeLightTask.m_chargeLight = chargeLight;

    if (!m_chargeLight) return;

    m_changeChargeLightTask.m_defaultColor = m_chargeLight->GetDiffuse();
    m_changeChargeLightTask.m_defaultIntensity = m_chargeLight->GetIntensity();
}

void PlayerBehavior::PlayPlayerEffect(PlayerEffectType type)
{
    if (!GAME_EFFECT || !CUSTOM_POST_EFFECT) return;

    switch (type) {
        // === Dodge ===
    case PlayerEffectType::DodgeStart:
        GAME_EFFECT->ChangeFOVTemporary(70.0f, 0.08f, 0.04f);
        m_afterimageGenerator->StartEmission();
        break;
    case PlayerEffectType::DodgeEnd:
        GAME_EFFECT->ResetFOV(0.1f);
        m_afterimageGenerator->StopEmission();
        break;
        // === Aim ===
    case PlayerEffectType::AimHoldStart:
        GAME_EFFECT->ChangeCameraLocalOffsetTemporary(XMFLOAT3(0.35f, 0.05f, 0.0f), 0.08f, 0.08f);
        break;

    case PlayerEffectType::AimStart:
        GAME_EFFECT->ChangeFOV(65.0f, 0.2f);
        GAME_EFFECT->ChangeCameraOffset(XMFLOAT3(0.0f, -0.5f, 0.0f), 0.1f);
        GAME_EFFECT->ChangeCameraLocalOffset(XMFLOAT3(2.5f, 0.0f, 0.0f), 0.1f);
        GAME_EFFECT->ChangeCameraDistance(5.0f, 0.1f);
        CUSTOM_POST_EFFECT->PlayEffect(CustomPostEffectType::MonoMask, 0.8f, 0.2f, MiMath::Infinity());
        break;

    case PlayerEffectType::AimEnd:
        GAME_EFFECT->ResetFOV(0.1f);
        GAME_EFFECT->ResetCameraOffset(0.1f);
        GAME_EFFECT->ResetCameraLocalOffset(0.1f);
        GAME_EFFECT->ResetCameraDistance(0.1f);
        CUSTOM_POST_EFFECT->PlayEffect(CustomPostEffectType::MonoMask, 0.0f, 0.1f, 0.0f);
        break;
        // === Single Attack ===
    case PlayerEffectType::SingleAttack:
        GAME_EFFECT->ChangeFOVTemporary(72.0f, 0.08f, 0.04f);
        GAME_EFFECT->ChangeCameraLocalOffsetTemporary(XMFLOAT3(0.45f, 0.0f, 0.10f), 0.06f, 0.04f);
        GAME_EFFECT->PlayCameraShake(0.10f, 0.15f);
        break;

    case PlayerEffectType::SingleHit:
        GAME_EFFECT->PlayCameraShake(0.08f, 0.20f);
        break;
        // === Charge Attack ===
    case PlayerEffectType::ChargeStart:
        GAME_EFFECT->ChangeFOV(60.0f, 0.2f);
        GAME_EFFECT->PlayCameraShake(0.08f, 0.10f);
        ChangeChargeLight(XMFLOAT4(0.35f, 0.75f, 1.0f, 1.0f), 8.0f, 1.0f);

        if (m_chargeEffect) {
            m_chargeEffect->Play();
            m_chargeEffect->Emission().enabled = true;
        }
        break;

    case PlayerEffectType::ChargeAttack:
        if (m_hitStopBehavior) {
            m_hitStopBehavior->StartHitStop(
                0.2f,
                [this]() {
                    GAME_EFFECT->ChangeFOVTemporary(78.0f, 0.10f, 0.06f);
                    GAME_EFFECT->ChangeCameraLocalOffsetTemporary(XMFLOAT3(0.15f, 0.0f, 0.25f), 0.08f, 0.08f);
                    GAME_EFFECT->PlayCameraShake(0.16f, 0.30f);

                    CUSTOM_POST_EFFECT->PlayEffect(CustomPostEffectType::RadialBlur, 0.6f, 0.2f, MiMath::Infinity());

                    // チャージライトを瞬間的に強くフラッシュ
                    m_chargeLight->SetEnable(true);
                    m_chargeLight->SetIntensity(20.0f);

                    if (m_chargeEffect) {
                        m_chargeEffect->Stop();
                        m_chargeEffect->Emission().enabled = false;
                    }

                    m_lockMovement = true;
                    if (m_spriteAnimation) {
                        m_spriteAnimation->Stop();
                    }
                    if (m_rigidbody) {
                        m_rigidbody->SetIsKinematic(true);
                    }
                },
                nullptr,
                nullptr,
                [this]() {
                    CUSTOM_POST_EFFECT->PlayEffect(CustomPostEffectType::RadialBlur, 0.0f, 0.1f, 0.0f);

                    ResetChargeLight(0.5f);

                    if (m_chargeEffect) {
                        m_chargeEffect->Play();
                        m_chargeEffect->Emission().enabled = false;
                    }

                    m_lockMovement = false;
                    if (m_rigidbody) {
                        m_rigidbody->SetIsKinematic(false);
                    }
                }
            );
        }
        break;

    case PlayerEffectType::ChargeHit:
        GAME_EFFECT->ChangeFOVTemporary(82.0f, 0.06f, 0.04f);
        GAME_EFFECT->PlayCameraShake(0.14f, 0.40f);
        break;

    case PlayerEffectType::AttackEnd:
        GAME_EFFECT->ResetFOV(0.12f);
        GAME_EFFECT->ResetCameraLocalOffset(0.12f);
        if (m_chargeEffect) {
            m_chargeEffect->Stop();
        }
        break;

    default:
        break;
    }
}

// -------------------------------------------------- private

// プレイヤーの入力処理
PlayerInput PlayerBehavior::UpdateInput()
{
    PlayerInput input = PlayerInput();

    // 移動入力
    if (Keyboard_IsKeyDown(KK_D)) {
        input.horizontal = 1.0f;
    }
    else if (Keyboard_IsKeyDown(KK_A)) {
        input.horizontal = -1.0f;
    }
    // 前後方向の移動入力
    if (Keyboard_IsKeyDown(KK_W)) {
        input.vertical = 1.0f;
    }
    else if (Keyboard_IsKeyDown(KK_S)) {
        input.vertical = -1.0f;
    }
    // カメラから見た移動入力の変換
    if (m_mainCamera) {
        XMFLOAT3 cameraForward = m_mainCamera->GetForward();
        XMFLOAT3 cameraRight = m_mainCamera->GetRight();

        input.moveInputCameraLocal = MiMath::Add(
            MiMath::Multiply(cameraRight, input.horizontal),
            MiMath::Multiply(cameraForward, input.vertical)
        );
    }

    // ジャンプ入力
    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        input.triggerJumpCommand = true;
    }

    // ダッシュ入力
    if (Keyboard_IsKeyDownTrigger(KK_LEFTSHIFT)) {
        input.triggerDashCommand = true;
    }

    // エイム入力
    if (Mouse_IsButtonDownTrigger(Mouse_Button::RIGHT)) {
        input.triggerAimCommand = true;
    }
    if (Mouse_IsButtonDown(Mouse_Button::RIGHT)) {
        input.holdAimCommand = true;
    }
    if (Mouse_IsButtonUpTrigger(Mouse_Button::RIGHT)) {
        input.releaseAimCommand = true;
    }
    
    // 攻撃入力
    if (Mouse_IsButtonDownTrigger(Mouse_Button::LEFT)) {
        input.triggerAttackCommand = true;
    }
    if (Mouse_IsButtonDown(Mouse_Button::LEFT)) {
        input.holdAttackCommand = true;
    }
    if (Mouse_IsButtonUpTrigger(Mouse_Button::LEFT)) {
        input.releaseAttackCommand = true;
    }

    return input;
}

// プレイヤーのアニメーション制御
void PlayerBehavior::UpdateAnimation(PlayerState state, PlayerCombatState combatState)
{
    if (!m_spriteAnimation) return;

    // 状態と戦闘状態に応じたアニメーションの再生
    std::string clipName;
    switch (state) {
    case PlayerState::Idle:
        clipName = "Idle";
        break;
    case PlayerState::Move:
        clipName = "Run";
        break;
    default:
        clipName = "Idle";
        break;
    }

    //// 戦闘状態に応じたアニメーションの上書き
    //if (combatState == PlayerCombatState::Aim) {
    //    clipName = "Aim";
    //}
    //else if (combatState == PlayerCombatState::Aim) {
    //    clipName = "Charge";
    //}
    //else if (combatState == PlayerCombatState::SingleAttack || combatState == PlayerCombatState::ChargeAttack) {
    //    clipName = "Attack";
    //}

    if (m_spriteAnimation->GetClip(clipName) == nullptr) return;
    if (m_spriteAnimation->GetClip(clipName) == m_spriteAnimation->GetCurrentClip()) return;

    m_spriteAnimation->Play(clipName);
}

// --------------------------------- チャージライトの変化管理

void PlayerBehavior::ChangeChargeLight(const XMFLOAT4& color, float intensity, float duration)
{
    if (!m_chargeLight) return;

    m_changeChargeLightTask.Reset();
    m_changeChargeLightTask.m_chargeLight = m_chargeLight;
    m_changeChargeLightTask.m_startColor = m_chargeLight->GetDiffuse();
    m_changeChargeLightTask.m_targetColor = color;
    m_changeChargeLightTask.m_endColor = color;
    m_changeChargeLightTask.m_startIntensity = m_chargeLight->GetIntensity();
    m_changeChargeLightTask.m_targetIntensity = intensity;
    m_changeChargeLightTask.m_endIntensity = intensity;
    m_changeChargeLightTask.m_duration = duration;
    m_changeChargeLightTask.m_holdDuration = 0.0f;
    m_changeChargeLightTask.Start();
}

void PlayerBehavior::ChangeChargeLightTemporary(const XMFLOAT4& color, float intensity, float duration, float holdDuration)
{
    if (!m_chargeLight) return;

    m_changeChargeLightTask.Reset();
    m_changeChargeLightTask.m_chargeLight = m_chargeLight;
    m_changeChargeLightTask.m_startColor = m_chargeLight->GetDiffuse();
    m_changeChargeLightTask.m_targetColor = color;
    m_changeChargeLightTask.m_endColor = m_changeChargeLightTask.m_defaultColor;
    m_changeChargeLightTask.m_startIntensity = m_chargeLight->GetIntensity();
    m_changeChargeLightTask.m_targetIntensity = intensity;
    m_changeChargeLightTask.m_endIntensity = m_changeChargeLightTask.m_defaultIntensity;
    m_changeChargeLightTask.m_duration = duration;
    m_changeChargeLightTask.m_holdDuration = holdDuration;
    m_changeChargeLightTask.Start();
}

void PlayerBehavior::ResetChargeLight(float duration)
{
    ChangeChargeLight(m_changeChargeLightTask.m_defaultColor, m_changeChargeLightTask.m_defaultIntensity, duration);
}


// ------------------------------------------------ PlayerBehavior::ChangeChargeLightTask

void PlayerBehavior::ChangeChargeLightTask::Start()
{
    SequenceTask::Start();

    if (m_chargeLight) {
        m_chargeLight->SetEnable(true); // ライトを有効化
        m_chargeLight->SetDiffuse(m_startColor);
        m_chargeLight->SetIntensity(m_startIntensity);
    }
}

void PlayerBehavior::ChangeChargeLightTask::Update(float deltaTime)
{
    if (IsFinished()) return;

    SequenceTask::Update(deltaTime);

    if (!m_chargeLight) {
        Finish();
        return;
    }

    switch (m_taskStep) {
    case 0: {
        float t = 1.0f;
        if (m_duration > 0.0f) {
            t = (std::min)(m_taskTimer / m_duration, 1.0f);
        }

        m_chargeLight->SetDiffuse(MiMath::Lerp(m_startColor, m_targetColor, t));
        m_chargeLight->SetIntensity(MiMath::Lerp(m_startIntensity, m_targetIntensity, t));

        if (t >= 1.0f) {
            if (m_holdDuration > 0.0f) {
                AdvanceStep();
            }
            else {
                // 目標値が0以下の場合はライトを無効化
                if (m_targetIntensity <= 0.0f) {
                    m_chargeLight->SetEnable(false);
                }
                Finish();
            }
        }
        break;
    }
    case 1:
        if (Wait(m_holdDuration)) {
            AdvanceStep();
        }
        break;

    case 2: {
        float t = 1.0f;
        if (m_duration > 0.0f) {
            t = (std::min)(m_taskTimer / m_duration, 1.0f);
        }

        m_chargeLight->SetDiffuse(MiMath::Lerp(m_targetColor, m_endColor, t));
        m_chargeLight->SetIntensity(MiMath::Lerp(m_targetIntensity, m_endIntensity, t));

        if (t >= 1.0f) {
            m_chargeLight->SetDiffuse(m_endColor);
            m_chargeLight->SetIntensity(m_endIntensity);

            // 終了値が0以下の場合はライトを無効化
            if (m_endIntensity <= 0.0f) {
                m_chargeLight->SetEnable(false);
            }
            Finish();
        }
        break;
    }
    default:
        Finish();
        break;
    }
}