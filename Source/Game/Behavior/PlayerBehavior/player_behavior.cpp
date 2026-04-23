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

#include "Engine/Editor/imgui_window_interface.h"

// プレイヤーが依存するComponentのヘッダ
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/animation_component.h"

// プレイヤーを構成する各種ビヘイビアのヘッダ
#include "./PlayerState/player_move_behavior.h"
#include "./PlayerState/player_attack_behavior.h"

#include "player_state_machine_behavior.h"
#include "player_combat_machine_behavior.h"

void PlayerBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;
    
    m_context.moveBehavior = owner->GetComponent<PlayerMoveBehavior>();
    m_context.attackBehavior = owner->GetComponent<PlayerAttackBehavior>();

    m_stateMachine = owner->GetComponent<PlayerStateMachineBehavior>();
    m_combatMachine = owner->GetComponent<PlayerCombatMachineBehavior>();
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
    PlayerInput input;
    input.moveX = 0.0f;
    input.moveZ = 0.0f;
    input.triggerJump = false;

    // 水平方向の移動入力
    if (Keyboard_IsKeyDown(KK_A)) {
       input.moveX = -1.0f;
    }
    else if (Keyboard_IsKeyDown(KK_D)) {
        input.moveX = 1.0f;
    }

    // 前後方向の移動入力
    if (Keyboard_IsKeyDown(KK_W)) {
        input.moveZ = 1.0f;
    }
    else if (Keyboard_IsKeyDown(KK_S)) {
        input.moveZ = -1.0f;
    }

    // ジャンプ入力
    input.triggerJump = Keyboard_IsKeyDownTrigger(KK_SPACE);

    return input;
}
