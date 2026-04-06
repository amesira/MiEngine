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
    m_myMaterial = m_model->GetMaterialSlots()[0].materialResource;

    // プレイヤーを構成する各種ビヘイビアの追加
    m_moveBehavior = owner->AddComponent<PlayerMoveBehavior>();
    m_attackBehavior = owner->AddComponent<PlayerAttackBehavior>();

}

void PlayerBehavior::Update()
{
    float deltaTime = FPS_GetDeltaTime();

    m_moveDirection = { 0.0f, 0.0f, 0.0f };

    if (Keyboard_IsKeyDown(KK_W)) {
        m_moveDirection.z = 1.0f;
    }
    if (Keyboard_IsKeyDown(KK_S)) {
        m_moveDirection.z = -1.0f;
    }
    if (Keyboard_IsKeyDown(KK_A)) {
        m_moveDirection.x = -1.0f;
    }
    if (Keyboard_IsKeyDown(KK_D)) {
        m_moveDirection.x = 1.0f;
    }

    // 速度設定
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    velocity.x = m_moveDirection.x * m_moveSpeed;
    velocity.z = m_moveDirection.z * m_moveSpeed;

    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        velocity.y = 5.0f;
    }

    // 速度方向に応じてプレイヤーの向きを変える
    if (velocity.x != 0.0f || velocity.z != 0.0f) {
        float angle = atan2f(m_moveDirection.x, m_moveDirection.z) + XM_PI;
        m_transform->SetEulerRotation({ 0.0f, angle, 0.0f });
    }

    m_rigidbody->SetVelocity(velocity);
}

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
