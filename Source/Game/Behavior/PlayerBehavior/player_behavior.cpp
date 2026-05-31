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

// コンポーネント
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/animation_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/sprite_animation_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"

// プレイヤーを構成する各種ビヘイビアのヘッダ
#include "player_state_machine_behavior.h"
#include "player_combat_machine_behavior.h"

#include "./PlayerState/player_move_behavior.h"
#include "./PlayerState/player_attack_behavior.h"
#include "./PlayerState/player_dodge_behavior.h"

#include "Game/ControllerBehavior/game_controller_locator.h"
#include "Game/ControllerBehavior/game_effect_controller.h"
#define GAME_EFFECT GameControllerLocator::GetGameEffectController()

void PlayerBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;

    m_spriteRenderer = owner->GetComponent<SpriteRendererComponent>();
    m_spriteAnimation = owner->GetComponent<SpriteAnimationComponent>();
    
    m_stateMachine = owner->GetComponent<PlayerStateMachineBehavior>();
    m_combatMachine = owner->GetComponent<PlayerCombatMachineBehavior>();

    m_context.moveBehavior = owner->GetComponent<PlayerMoveBehavior>();
    m_context.attackBehavior = owner->GetComponent<PlayerAttackBehavior>();
    m_context.dodgeBehavior = owner->GetComponent<PlayerDodgeBehavior>();

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

    // 入力の更新
    m_context.input = UpdateInput();

    // 状態マシーンの更新
    if (m_stateMachine) {
        m_stateMachine->UpdateStateMachine(m_context, deltaTime);
    }

    // 戦闘マシーンの更新
    if (m_combatMachine) {
        m_combatMachine->UpdateCombatMachine(m_context, deltaTime);
    }

    // テスト：GameEffectControllerのタイムスケール変更タスクをスペースキーで開始
    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        if (GAME_EFFECT) {
            GAME_EFFECT->ChangeTimeScaleTemporary(0.5f, 0.5f, 1.0f); // タイムスケールを0.5にして、0.5秒かけて元に戻す
            GAME_EFFECT->ChangeFOVTemporary(90.0f, 0.5f, 1.0f); // FOVを90にして、0.5秒かけて元に戻す
            GAME_EFFECT->ChangeCameraDistanceTemporary(5.0f, 0.5f, 1.0f); // カメラ距離を5にして、0.5秒かけて元に戻す
            GAME_EFFECT->ChangeCameraOffsetTemporary(XMFLOAT3(0.0f, -1.0f, 0.0f), 0.5f, 1.0f); // カメラオフセットを(0,-1,0)にして、0.5秒かけて元に戻す

            GAME_EFFECT->PlayCameraShake(0.5f, 1.0f); // 0.5秒間、強さ0.3のカメラシェイクを再生
        }
    }

    // アニメーション制御
    UpdateAnimation(m_context.state, m_context.combatState);

    // Flip
    if (m_context.input.horizontal > 0.01f) {
        m_spriteRenderer->SetFlipX(true);
    }
    else if (m_context.input.horizontal < -0.01f) {
        m_spriteRenderer->SetFlipX(false);
    }
}

// PlayerBehaviorのインスペクタ表示
void PlayerBehavior::DrawComponentInspector()
{
    if(InspectorViewWindow::BeginComponentSection(this, "Player Behavior")) {
        // 参照状態の表示
        ImGui::Text("StateMachine: %s", m_stateMachine ? "OK" : "None");
        ImGui::Text("CombatMachine: %s", m_combatMachine ? "OK" : "None");

        // 状態の表示
        ImGui::Text("State: %s", [this]() {
            switch (m_context.state) {
            case PlayerState::Idle: return "Idle";
            case PlayerState::Move: return "Move";
            case PlayerState::Attack: return "Attack";
            case PlayerState::Dodge: return "Dodge";
            case PlayerState::Stunned: return "Stunned";
            default: return "Unknown";
            }
            }());

        // 入力状態の表示
        ImGui::Text("MoveInputCameraLocal: (%.2f, %.2f, %.2f)", m_context.input.moveInputCameraLocal.x, m_context.input.moveInputCameraLocal.y, m_context.input.moveInputCameraLocal.z);

    }

    InspectorViewWindow::EndComponentSection();
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
    //else if (combatState == PlayerCombatState::Charge) {
    //    clipName = "Charge";
    //}
    //else if (combatState == PlayerCombatState::Attack) {
    //    clipName = "Attack";
    //}

    if (m_spriteAnimation->GetClip(clipName) == nullptr) return;
    if (m_spriteAnimation->GetClip(clipName) == m_spriteAnimation->GetCurrentClip()) return;

    m_spriteAnimation->Play(clipName);
}