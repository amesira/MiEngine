//===================================================
// enemy_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/06
//===================================================
#include "enemy_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"

#include "Engine/Framework/Component/camera_component.h"

#include "Game/Behavior/BaseBehavior/health_behavior.h"
#include "Game/Behavior/BaseBehavior/hit_stop_behavior.h"
#include "Game/Behavior/BaseBehavior/blinker_behavior.h"
#include "Game/Behavior/BaseBehavior/shake_object_behavior.h"

#include "Utility/mi_math.h"

#include "enemy_state_machine_behavior.h"
#include "base_enemy_attack_behavior.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

using namespace DirectX;

namespace {
    // デバッグ用：EnemyStateを文字列に変換
    const char* ToEnemyStateName(EnemyState state)
    {
        switch (state) {
        case EnemyState::Idle: return "Idle";
        case EnemyState::Chase: return "Chase";
        case EnemyState::Attack: return "Attack";
        case EnemyState::Stunned: return "Stunned";
        case EnemyState::Dead: return "Dead";
        default: return "Unknown";
        }
    }
}

void EnemyBehavior::Start()
{
    GameObject* owner = GetOwner();
    if (!owner) return;

    // コンテキストの初期化
    m_context.enemyBehavior = this;
    m_context.transform = owner->GetComponent<TransformComponent>();
    m_context.rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_context.stateMachine = owner->GetComponent<EnemyStateMachineBehavior>();
    m_context.attackBehavior = owner->GetComponent<BaseEnemyAttackBehavior>();

    if (m_context.attackBehavior) {
        m_context.attackType = m_context.attackBehavior->GetAttackType();
    }

    m_hitStopBehavior = owner->GetComponent<HitStopBehavior>();
    m_healthBehavior = owner->GetComponent<HealthBehavior>();
    m_healthBehavior->SetMaxHealth(30.0f, true);

    m_blinkerBehavior = owner->GetComponent<BlinkerBehavior>();
    m_shakeObjectBehavior = owner->GetComponent<ShakeObjectBehavior>();

    // ダメージコールバックの設定
    m_healthBehavior->SetOnTakeDamageCallback([this](float currentHealth, float damage) {
        if (currentHealth <= 0.0f) {
            m_context.state = EnemyState::Dead; // 仮
            if (m_hitStopBehavior) {
                m_hitStopBehavior->StartHitStop(
                    0.5f,
                    [this]() {
                        m_blinkerBehavior->Flash({ 1.0f, 0.0f, 0.0f }, 1.0f, 0.5f); // 死亡時に赤くフラッシュ
                        m_shakeObjectBehavior->ShakeTemporary(0.5f, 1.0f); // 死亡時に揺らす（オフセットリセットあり）
                    },
                    nullptr,
                    nullptr,
                    [this]() {
                        if (GetOwner()) {
                            GetOwner()->Destroy(); // ヒットストップ終了後にオブジェクトを破棄
                        }
                    }); // 死亡時に長めのヒットストップを開始
            }
        }
        else {
            m_context.state = EnemyState::Stunned; // 仮
            if (m_hitStopBehavior) {
                m_hitStopBehavior->StartHitStop(
                    0.3f,
                    [this]() {
                        m_blinkerBehavior->Flash({ 1.0f, 0.1f, 0.1f }, 1.0f, 0.3f); // ダメージを受けたときに短く赤くフラッシュ
                        m_shakeObjectBehavior->Shake(0.3f, 1.0f); // ダメージを受けたときに短く揺らす
                    },
                    nullptr,
                    nullptr,
                    [this]() {
                        m_blinkerBehavior->Reset(0.3f); // ヒットストップ終了後にフラッシュをリセット
                        m_shakeObjectBehavior->Reset(0.3f); // ヒットストップ終了後に揺れをリセット

                        m_context.state = EnemyState::Chase; // ヒットストップ終了後にChase状態に戻す（仮）
                    }); // ダメージを受けたときに短いヒットストップを開始
            }
        }
    });

    IScene* scene = owner->GetScene();
    if (scene) {
        // ターゲットオブジェクト（プレイヤー）をシーンから取得
        m_context.targetObject = scene->GetGameObjectByName("Player");
        if (m_context.targetObject) {
            m_context.targetTransform = m_context.targetObject->GetComponent<TransformComponent>();
        }

        GameObject* mainCameraObj = scene->GetGameObjectByName("MainCamera");
        if (mainCameraObj) {
            m_mainCamera = mainCameraObj->GetComponent<CameraComponent>();
        }
    }
}

void EnemyBehavior::Update()
{
    // ターゲットとの距離や視認状態を毎フレーム更新
    UpdateTargetInformation();

    // 状態マシンの更新
    const float deltaTime = FPS_GetDeltaTime();
    if (m_context.stateMachine) {
        m_context.stateMachine->UpdateStateMachine(m_context, deltaTime);
    }

    // 回転処理
    XMFLOAT3 forward = m_mainCamera->GetForward();

    // 正面方向からY軸回転の角度を計算
    const float billboardAngleY = atan2f(forward.x, forward.z);
    XMFLOAT4 targetRotation = MiMath::QuaternionFromEuler({ 0.0f, billboardAngleY, 0.0f });
    XMFLOAT4 currentRotation = m_context.transform->GetRotation();
    currentRotation = MiMath::Slerp(currentRotation, targetRotation, 20.0f * deltaTime);

    m_context.transform->SetRotation(currentRotation);
    m_currentAngleY = m_context.transform->GetEulerAngle().y;
}

void EnemyBehavior::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Enemy Behavior")) {
        ImGui::Text("State: %s", ToEnemyStateName(m_context.state));
        ImGui::Text("Target: %s", m_context.targetObject ? m_context.targetObject->GetName().c_str() : "None");
        ImGui::Text("Distance To Target: %.2f", m_context.distanceToTarget);
        ImGui::Text("Can See Target: %s", m_context.canSeeTarget ? "true" : "false");
    }

    InspectorViewWindow::EndComponentSection();
}

// ------------------------------------------------ private

// ターゲットとの距離や視認状態を更新する処理
void EnemyBehavior::UpdateTargetInformation()
{
    m_context.distanceToTarget = 0.0f;
    m_context.canSeeTarget = false;

    if (!m_context.transform || !m_context.targetTransform) return;

    // 敵とターゲットの位置から距離を計算
    const XMFLOAT3 enemyPosition = m_context.transform->GetPosition();
    const XMFLOAT3 targetPosition = m_context.targetTransform->GetPosition();
    const XMFLOAT3 toTarget = MiMath::Subtract(targetPosition, enemyPosition);

    m_context.distanceToTarget = MiMath::Length(toTarget);
    m_context.canSeeTarget = true; // 仮：常に視認できるとする。実際にはレイキャストなどで障害物を考慮して判定する。
}
