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
#include "Engine/Editor/imgui_window_interface.h"
#include "Engine/Editor/inspector_view_window.h"

#include "Engine/Framework/Component/transform_component.h"

#include "Game/ControllerBehavior/game_controller_locator.h"
#include "Game/ControllerBehavior/game_effect_controller.h"

#define GAME_EFFECT_CONTROLLER GameControllerLocator::GetGameEffectController()

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
    if (InspectorViewWindow::BeginComponentSection(this, "Player Visual Machine")) {
        
    }
    InspectorViewWindow::EndComponentSection();
}

// ------------------------------- public

void PlayerVisualMachineBehavior::UpdateVisualMachine(PlayerContext& context, float deltaTime)
{
    // Move：移動中は向きと傾きを更新
    if (context.state == PlayerState::Move) {
        UpdateTilt(context, deltaTime);
    }
}

// ------------------------------- private

// 傾き更新処理
void PlayerVisualMachineBehavior::UpdateTilt(const PlayerContext& context, float deltaTime)
{
    float angleDiff = MiMath::Angle(
        m_transform->GetForward(),
        context.input.moveInputCameraLocal
    );

    if(MiMath::Length(context.input.moveInputCameraLocal) > 0.01f) {
        XMFLOAT3 targetEuler = {
            m_forwardTilt.x,
            m_transform->GetEulerAngle().y,
            0.0f,
        };
        XMFLOAT3 currentEuler = m_transform->GetEulerAngle();

        // スムーズに傾きを更新
        currentEuler = MiMath::Lerp(currentEuler, targetEuler, deltaTime * 10.0f);

        m_transform->SetEulerAngle(currentEuler);
    }
     else {
        // 入力がないときは傾きを元に戻す
        XMFLOAT3 currentEuler = m_transform->GetEulerAngle();
        currentEuler = MiMath::Lerp(currentEuler, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, deltaTime * 10.0f);
        m_transform->SetEulerAngle(currentEuler);
    }

}
