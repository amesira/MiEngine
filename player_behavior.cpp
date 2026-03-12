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

#include "transform_component.h"
#include "rigidbody_component.h"
#include "model_component.h"

#include "imgui_window_interface.h"

void PlayerBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;
    
    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_model = owner->GetComponent<ModelComponent>();
}

void PlayerBehavior::Update()
{
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

    m_rigidbody->SetVelocity(velocity);

    m_model->SetColor(m_color);

    hal::dout << "Velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
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
        if (ImGui::ColorEdit4("Color", &m_color.x)) {
            
        }
    }

    if (!prevEnable) {
        ImGui::PopStyleVar();
    }
}
