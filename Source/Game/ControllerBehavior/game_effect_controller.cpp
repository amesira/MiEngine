//===================================================
// game_effect_controller.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/27
//===================================================
#include "game_effect_controller.h"

#include "Engine/Device/mi_fps.h"
#include "game_controller_locator.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"
#include "Engine/Editor/EditorWindow/inspector_view_window.h"

#include "Utility/mi_math.h"

#include "Utility/debug_ostream.h"

#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Game/Behavior/camera_control_behavior.h"

GameEffectController::GameEffectController()
{
    s_instanceCount++;

    if (s_instanceCount > 1) {
        hal::dout << "警告: GameEffectControllerのインスタンスが複数存在しています。ゲーム全体で1つだけ存在することを想定しています。" << std::endl;
        this->SetEnable(false);
    }
    else {
        GameControllerLocator::s_gameEffectController = this;
    }

    // タスクの初期化
    m_changeTimeScaleTask.Reset();
}

GameEffectController::~GameEffectController()
{
    if (GameControllerLocator::s_gameEffectController == this) {
        GameControllerLocator::s_gameEffectController = nullptr;
    }

    s_instanceCount--;
}

void GameEffectController::Start()
{
    IScene* scene = GetOwner()->GetScene();

    // カメラコントロールビヘイビアへの参照取得
    {
        GameObject* mainCamera = scene->GetGameObjectByName("MainCamera");
        if (mainCamera) {
            m_cameraControl = mainCamera->GetComponent<CameraControlBehavior>();
        }
    }
}

void GameEffectController::Update()
{
    float unscaledDeltaTime = FPS_GetUnscaledDeltaTime();

    // タスクの更新
    m_changeTimeScaleTask.Update(unscaledDeltaTime);
}

void GameEffectController::DrawComponentInspector()
{
    if (InspectorViewWindow::BeginComponentSection(this, "Game Effect Controller")) {
        if (ImGui::TreeNode("Time Scale")) {
            static float timeScale = 0.2f;
            static float duration = 0.15f;
            static float holdDuration = 0.2f;

            ImGui::DragFloat("Target Time Scale", &timeScale, 0.01f, 0.0f, 2.0f);
            ImGui::DragFloat("Duration", &duration, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Hold Duration", &holdDuration, 0.01f, 0.0f, 5.0f);

            if (ImGui::Button("Change Time Scale")) {
                ChangeTimeScale(timeScale, duration);
            }

            ImGui::SameLine();

            if (ImGui::Button("Temporary")) {
                ChangeTimeScaleTemporary(timeScale, duration, holdDuration);
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset")) {
                ResetTimeScale(duration);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Camera FOV")) {
            static float fov = 60.0f;
            static float duration = 0.2f;
            static float holdDuration = 0.2f;

            ImGui::DragFloat("FOV", &fov, 1.0f, 1.0f, 179.0f);
            ImGui::DragFloat("Duration", &duration, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Hold Duration", &holdDuration, 0.01f, 0.0f, 5.0f);

            if (ImGui::Button("Change FOV")) {
                ChangeFOV(fov, duration);
            }

            ImGui::SameLine();

            if (ImGui::Button("Temporary")) {
                ChangeFOVTemporary(fov, duration, holdDuration);
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset")) {
                ResetFOV(duration);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Camera Distance")) {
            static float distance = 10.0f;
            static float duration = 0.2f;
            static float holdDuration = 0.2f;

            ImGui::DragFloat("Distance", &distance, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("Duration", &duration, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Hold Duration", &holdDuration, 0.01f, 0.0f, 5.0f);

            if (ImGui::Button("Change Distance")) {
                ChangeCameraDistance(distance, duration);
            }

            ImGui::SameLine();

            if (ImGui::Button("Temporary")) {
                ChangeCameraDistanceTemporary(distance, duration, holdDuration);
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset")) {
                ResetCameraDistance(duration);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Camera Shake")) {
            static float duration = 0.2f;
            static float magnitude = 0.2f;

            ImGui::DragFloat("Duration", &duration, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Magnitude", &magnitude, 0.01f, 0.0f, 5.0f);

            if (ImGui::Button("Play Camera Shake")) {
                PlayCameraShake(duration, magnitude);
            }

            ImGui::TreePop();
        }
    }

    InspectorViewWindow::EndComponentSection();
}

//------------------------------- private

// タイムスケール変更タスクの更新
void GameEffectController::ChangeTimeScaleTask::Update(float deltaTime)
{
    if (!m_isRunning) return;
    SequenceTask::Update(deltaTime);

    switch(m_taskStep) {
        case 0: {
            // タイムスケールを目標値に向けて変化させる
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }
            float newTimeScale = MiMath::Lerp(m_startTimeScale, m_targetTimeScale, t);
            FPS_SetTimeScale(newTimeScale);

            if (t >= 1.0f) {
                AdvanceStep();
            }
            break;
        }
        case 1: {
            if (m_holdDuration > 0.0f) {
                // 一定時間保持
                if (Wait(m_holdDuration, deltaTime)) {
                    AdvanceStep();
                }
            }
            else {
                Finish(); // 保持なしで完了
            }
            break;
        }
        case 2: {
            // タイムスケールを元に戻す
            float t = 1.0f;
            if (m_duration > 0.0f) {
                t = (std::min)(m_taskTimer / m_duration, 1.0f);
            }
            float newTimeScale = MiMath::Lerp(m_targetTimeScale, m_defaultTimeScale, t);
            FPS_SetTimeScale(newTimeScale);

            if (t >= 1.0f) {
                FPS_SetTimeScale(m_defaultTimeScale); // 確実に元のタイムスケールに戻す
                Finish();
            }
            break;
        }
    }
}


//------------------------------- public

// タイムスケール変更
void GameEffectController::ChangeTimeScale(float timeScale, float duration)
{
    m_changeTimeScaleTask.Reset();

    m_changeTimeScaleTask.m_startTimeScale = FPS_GetTimeScale();
    m_changeTimeScaleTask.m_targetTimeScale = timeScale;
    m_changeTimeScaleTask.m_duration = duration;
    m_changeTimeScaleTask.m_holdDuration = 0.0f;
    m_changeTimeScaleTask.Start();
}
void GameEffectController::ChangeTimeScaleTemporary(float timeScale, float duration, float holdDuration)
{
    m_changeTimeScaleTask.Reset();

    m_changeTimeScaleTask.m_startTimeScale = FPS_GetTimeScale();
    m_changeTimeScaleTask.m_targetTimeScale = timeScale;
    m_changeTimeScaleTask.m_duration = duration;
    m_changeTimeScaleTask.m_holdDuration = holdDuration;
    m_changeTimeScaleTask.Start();
}
// タイムスケールを元に戻す
void GameEffectController::ResetTimeScale(float duration)
{
    ChangeTimeScale(1.0f, duration);
}

// CameraControlBehaviorを介した実装の窓口

// FOV変更
void GameEffectController::ChangeFOV(float fov, float duration)
{
    if (m_cameraControl) {
        m_cameraControl->ChangeFOV(fov, duration);
    }
}
void GameEffectController::ChangeFOVTemporary(float fov, float duration, float holdDuration)
{
    if (m_cameraControl) {
        m_cameraControl->ChangeFOVTemporary(fov, duration, holdDuration);
    }
}
// FOVを元に戻す
void GameEffectController::ResetFOV(float duration)
{
    if (m_cameraControl) {
        m_cameraControl->ResetFOV(duration);
    }
}

// カメラ距離変更
void GameEffectController::ChangeCameraDistance(float distance, float duration)
{
    if (m_cameraControl) {
        m_cameraControl->ChangeCameraDistance(distance, duration);
    }
}
void GameEffectController::ChangeCameraDistanceTemporary(float distance, float duration, float holdDuration)
{
    if (m_cameraControl) {
        m_cameraControl->ChangeCameraDistanceTemporary(distance, duration, holdDuration);
    }
}
// カメラ距離を元に戻す
void GameEffectController::ResetCameraDistance(float duration)
{
    if (m_cameraControl) {
        m_cameraControl->ResetCameraDistance(duration);
    }
}

// カメラオフセット変更
void GameEffectController::ChangeCameraOffset(const XMFLOAT3& offset, float duration)
{
    if (m_cameraControl) {
        m_cameraControl->ChangeCameraOffset(offset, duration);
    }
}
void GameEffectController::ChangeCameraOffsetTemporary(const XMFLOAT3& offset, float duration, float holdDuration)
{
    if (m_cameraControl) {
        m_cameraControl->ChangeCameraOffsetTemporary(offset, duration, holdDuration);
    }
}
// カメラオフセットを元に戻す
void GameEffectController::ResetCameraOffset(float duration)
{
    if (m_cameraControl) {
        m_cameraControl->ResetCameraOffset(duration);
    }
}

// カメラシェイク再生
void GameEffectController::PlayCameraShake(float duration, float magnitude)
{
    if (m_cameraControl) {
        m_cameraControl->PlayCameraShake(duration, magnitude);
    }
}