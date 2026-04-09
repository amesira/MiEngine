//===================================================
// player_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "player_behavior.h"
#include "scene_interface.h"
#include "game_object.h"

#include "mi_math.h"
#include "keyboard.h"
#include "debug_ostream.h"
#include "mi_fps.h"

#include "imgui_window_interface.h"
#include "material_repository.h"

// プレイヤーが依存するComponentのヘッダ
#include "transform_component.h"
#include "rigidbody_component.h"
#include "model_component.h"
#include "animation_component.h"

// プレイヤーを構成する各種ビヘイビアのヘッダ
#include "player_move_behavior.h"
#include "player_attack_behavior.h"


void PlayerBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;
    
    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_model = owner->GetComponent<ModelComponent>();
    m_animation = owner->GetComponent<AnimationComponent>();

    m_myMaterial = m_model->GetMaterialSlots()[0].materialResource;

    // プレイヤーを構成する各種ビヘイビアの追加
    m_moveBehavior = owner->AddComponent<PlayerMoveBehavior>();
    m_attackBehavior = owner->AddComponent<PlayerAttackBehavior>();

    m_rigidbody->SetMass(3.0f);
    m_rigidbody->SetFriction({ 0.8f, 1.0f, 0.8f });
}

void PlayerBehavior::Update()
{
    float deltaTime = FPS_GetDeltaTime();

    // 入力の更新
    m_input = UpdateInput(m_input);

    XMFLOAT3 velocity = m_rigidbody->GetVelocity();

    // 移動更新処理
    {
        XMFLOAT3 moveDirection = { m_input.moveX , 0.0f, m_input.moveZ };
        moveDirection = MiMath::Multiply(MiMath::Normalize(moveDirection), m_moveSpeed);

        velocity.x += moveDirection.x * deltaTime * 10.0f; // 加速度的に速度を増加させるために10倍する
        velocity.z += moveDirection.z * deltaTime * 10.0f;

        velocity.x = MiMath::Clamp(velocity.x, -m_moveSpeed, m_moveSpeed);
        velocity.z = MiMath::Clamp(velocity.z, -m_moveSpeed, m_moveSpeed);
    }
    
    // ジャンプ処理
    {
        if (m_input.triggerJump) {
            velocity.y = m_jumpForce;
        }
    }

    // 速度方向に応じてプレイヤーの向きを変える
    if (MiMath::Length(XMFLOAT3(velocity.x, 0.0f, velocity.z)) > 0.2f) {
        float angle = atan2f(velocity.x, velocity.z) + XM_PI;
        float newAngleY = MiMath::Lerp(m_currentAngleY, angle, std::clamp(deltaTime * 10.0f, 0.0f, 1.0f));
        if (std::abs(newAngleY - m_currentAngleY) > XM_PI) {
            // 角度差が180度を超える場合は反対方向に回転する
            if (newAngleY > m_currentAngleY) {
                newAngleY -= XM_PI * 2.0f;
            }
            else {
                newAngleY += XM_PI * 2.0f;
            }
        }

        m_transform->SetEulerRotation({ 0.0f, newAngleY, 0.0f });

        m_currentAngleY = newAngleY;

        m_animation->SetAnimationState(0, 1.5f);
    }
    else {
        m_animation->SetAnimationState(1, 1.0f);
    }

    // 適用処理
    m_rigidbody->SetVelocity(velocity);
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

    // PlayerBehaviorのプロパティ表示
    if (ImGui::CollapsingHeader("Player Behavior", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("Enable", &enable)) {
            this->SetEnable(enable);
        }

        if (ImGui::DragFloat("Move Speed", &m_moveSpeed, 0.1f, 0.0f, 100.0f)) {
            m_moveSpeed = (std::max)(0.0f, m_moveSpeed);
        }

        if (m_myMaterial) {
            if (ImGui::DragFloat("Metallic", &m_myMaterial->metallic, 0.01f, 0.0f, 1.0f)) {
                m_myMaterial->metallic = (std::clamp)(m_myMaterial->metallic, 0.0f, 1.0f);
            }
            if (ImGui::DragFloat("Roughness", &m_myMaterial->roughness, 0.01f, 0.0f, 1.0f)) {
                m_myMaterial->roughness = (std::clamp)(m_myMaterial->roughness, 0.0f, 1.0f);
            }
        }
        else {
            ImGui::Text("No material found. Please Game Start.");
        }
    }

    if (!prevEnable) {
        ImGui::PopStyleVar();
    }
}

// --------------------------------------------------

// プレイヤーの入力処理
PlayerBehavior::PlayerInput PlayerBehavior::UpdateInput(const PlayerInput& currentInput)
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
