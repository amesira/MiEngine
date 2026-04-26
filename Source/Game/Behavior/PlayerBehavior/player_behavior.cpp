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
#include "Engine/System/Device/mi_fps.h"
#include "Engine/System/Device/keyboard.h"
#include "Engine/System/Device/mouse.h"

#include "Engine/Editor/imgui_window_interface.h"

// コンポーネント
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/animation_component.h"
#include "Engine/Framework/Component/camera_component.h"

// プレイヤーを構成する各種ビヘイビアのヘッダ
#include "player_state_machine_behavior.h"
#include "player_combat_machine_behavior.h"
#include "player_visual_machine_behavior.h"

#include "./PlayerState/player_move_behavior.h"
#include "./PlayerState/player_attack_behavior.h"

void PlayerBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;
    
    m_context.moveBehavior = owner->GetComponent<PlayerMoveBehavior>();
    m_context.attackBehavior = owner->GetComponent<PlayerAttackBehavior>();

    m_stateMachine = owner->GetComponent<PlayerStateMachineBehavior>();
    m_combatMachine = owner->GetComponent<PlayerCombatMachineBehavior>();
    m_visualMachine = owner->GetComponent<PlayerVisualMachineBehavior>();

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

    // ビジュアルマシーンの更新
    if (m_visualMachine) {
        m_visualMachine->UpdateVisualMachine(m_context, deltaTime);
    }
}

// PlayerBehaviorのインスペクタ表示
void PlayerBehavior::DrawComponentInspector()
{
    // Enableの表示
    bool enable = this->GetEnable();
    if (!enable) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
    }
    bool prevEnable = enable;

    if (!prevEnable) {
        ImGui::PopStyleVar();
    }
}

// --------------------------------------------------

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
        XMFLOAT3 cameraForward = MiMath::Normalize(MiMath::Subtract(m_mainCamera->GetAtPosition(), m_mainCamera->GetEyePosition()));
        XMFLOAT3 cameraRight = MiMath::Normalize(MiMath::Cross(cameraForward, m_mainCamera->GetUpVector()));
        cameraRight = MiMath::Multiply(cameraRight, -1.0f);

        input.moveInput = MiMath::Add(
            MiMath::Multiply(cameraRight, input.horizontal),
            MiMath::Multiply(cameraForward, input.vertical)
        );
    }


    // ジャンプ入力
    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        input.triggerJumpCommand = true;
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
