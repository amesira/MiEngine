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
#include "Engine/Framework/Component/animation_component.h"

#include "Game/ControllerBehavior/game_controller_locator.h"
#include "Game/ControllerBehavior/game_effect_controller.h"

#define GAME_EFFECT_CONTROLLER GameControllerLocator::GetGameEffectController()

void PlayerVisualMachineBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;

    m_transform = owner->GetComponent<TransformComponent>();
    m_animation = owner->GetComponent<AnimationComponent>();
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

// プレイヤーの見た目状態更新処理
void PlayerVisualMachineBehavior::UpdateVisualMachine(PlayerContext& context, float deltaTime)
{
    // Move：移動中は向きと傾きを更新
    if (context.state == PlayerState::Move) {
        UpdateTilt(context, deltaTime);
    }

    if (context.state == PlayerState::Idle) {
        m_animation->SetAnimationState(1, 1.0f);
    }
    else {
        m_animation->SetAnimationState(0, 1.0f);
    }

    if (!GAME_EFFECT_CONTROLLER) return;

    // GameEffectのTrigger処理
    TriggerSlowMotion(context.visualRequest.slowMotionRequest);
    TriggerEffect(context.visualRequest.effectRequest);
    TriggerPostEffectColor(context.visualRequest.postEffectColorRequest);
    TriggerPostEffectScreen(context.visualRequest.postEffectScreenRequest);

    // GameEffectの更新処理
    UpdateSlowMotion(deltaTime);
}

// スローモーションのトリガー
void PlayerVisualMachineBehavior::TriggerSlowMotion(SlowMotionRequest& request)
{
    if (request.trigger) {
        m_isInSlowMotion = true;
        m_slowMotionTimer = 0.0f;
        m_slowMotionDuration = request.duration;
        m_slowMotionFactor = request.scale;

        request.trigger = false; // トリガーは処理したらリセット
    }
}

// エフェクトのトリガー
void PlayerVisualMachineBehavior::TriggerEffect(EffectRequest& request)
{
    if (request.trigger) {
        // ToDo：エフェクトのトリガー処理を書く
        request.trigger = false; // トリガーは処理したらリセット
    }
}

// ポストエフェクト（色相）のトリガー
void PlayerVisualMachineBehavior::TriggerPostEffectColor(PostEffectColorRequest& request)
{
    if (request.trigger) {
        // ToDo：ポストエフェクト（色相）のトリガー処理を書く
        request.trigger = false; // トリガーは処理したらリセット
    }
}

// ポストエフェクト（画面効果）のトリガー
void PlayerVisualMachineBehavior::TriggerPostEffectScreen(PostEffectScreenRequest& request)
{
    if (request.trigger) {
        // ToDo：ポストエフェクト（画面効果）のトリガー処理を書く
        request.trigger = false; // トリガーは処理したらリセット
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

// スローモーション更新処理
bool PlayerVisualMachineBehavior::UpdateSlowMotion(float deltaTime)
{
    if (!m_isInSlowMotion) return false;

    m_slowMotionTimer += deltaTime / m_slowMotionFactor;

    // スローモーション終了処理
    if (m_slowMotionTimer >= m_slowMotionDuration) {
        GAME_EFFECT_CONTROLLER->CancelTimeScaleChange(0);
        m_isInSlowMotion = false;
        return false;
    }
    // スローモーション継続処理
    else {
        GAME_EFFECT_CONTROLLER->RequestTimeScaleChange(m_slowMotionFactor, m_slowMotionDuration - m_slowMotionTimer);
        return true;
    }
}
