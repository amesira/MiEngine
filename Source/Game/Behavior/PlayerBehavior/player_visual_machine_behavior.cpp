//===================================================
// player_visual_machine_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/26
//===================================================
#include "player_visual_machine_behavior.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Utility/mi_math.h"

#include "Engine/Framework/Component/transform_component.h"

void PlayerVisualMachineBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;

    m_transform = owner->GetComponent<TransformComponent>();
}

void PlayerVisualMachineBehavior::Update()
{

}

void PlayerVisualMachineBehavior::DrawComponentInspector()
{

}

// ------------------------------- public

void PlayerVisualMachineBehavior::UpdateVisualMachine(PlayerContext& context, float deltaTime)
{
    if (context.state == PlayerState::Move) {
        UpdateFacingDirection(context, deltaTime);
        UpdateTilt(context, deltaTime);
    }
}

// ------------------------------- private

// 向き更新処理
void PlayerVisualMachineBehavior::UpdateFacingDirection(const PlayerContext& context, float deltaTime)
{
    float angle = MiMath::Angle({ 0.0f, 0.0f, 1.0f }, context.input.moveInput);

    // 現在角度から目標角度への差分を -PI ～ PI に収めて、最短方向へ補間する
    float deltaAngle = angle - m_currentAngleY;
    while (deltaAngle > XM_PI) {
        deltaAngle -= XM_2PI;
    }
    while (deltaAngle < -XM_PI) {
        deltaAngle += XM_2PI;
    }

    float t = std::clamp(deltaTime * 10.0f, 0.0f, 1.0f);
    float newAngleY = m_currentAngleY + deltaAngle * t;

    m_transform->SetEulerAngle({ 0.0f, newAngleY, 0.0f });
    m_currentAngleY = newAngleY;
}

// 傾き更新処理
void PlayerVisualMachineBehavior::UpdateTilt(const PlayerContext& context, float deltaTime)
{

}
