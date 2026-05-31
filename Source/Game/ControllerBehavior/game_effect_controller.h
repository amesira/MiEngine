//---------------------------------------------------
// game_effect_controller.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/27
//---------------------------------------------------
#ifndef GAME_EFFECT_CONTROLLER_H
#define GAME_EFFECT_CONTROLLER_H
#include "Engine/Framework/Component/behavior_component.h"
#include "Engine/Device/direct3d.h"
using namespace DirectX;
#include "Engine/Core/GamePlay/sequence_task.h"

class GameEffectController : public BehaviorComponent {
private:
    static inline int s_instanceCount = 0;

public:
    GameEffectController();
    ~GameEffectController();

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

private:
    // タイムスケール変更タスク
    class ChangeTimeScaleTask : public SequenceTask {
    public:
        // タイムスケールの開始値と目標値
        float m_startTimeScale = 1.0f;
        float m_targetTimeScale = 1.0f;
        // 時間経過の管理
        float m_duration;
        float m_holdDuration;

        void Update(float deltaTime) override;
    };
    ChangeTimeScaleTask m_changeTimeScaleTask;

public:
    // タイムスケール変更
    void ChangeTimeScale(float timeScale, float duration);
    void ChangeTimeScaleTemporary(float timeScale, float duration, float holdDuration);
    // タイムスケールを元に戻す
    void ResetTimeScale(float duration);

    // カメラシェイク再生
    //void PlayCameraShake(float duration, float magnitude);

    //// FOV変更
    //void ChangeFOV(float fov, float duration);
    //void ChangeFOVTemporary(float fov, float duration, float holdDuration);
    //// FOVを元に戻す
    //void ResetFOV(float duration);

    //// カメラ距離変更
    //void ChangeCameraDistance(float distance, float duration);
    //// カメラ距離を元に戻す
    //void ResetCameraDistance(float duration);

    //// カメラオフセット変更
    //void ChangeCameraOffset(const XMFLOAT3& offset, float duration);
    //// カメラオフセットを元に戻す
    //void ResetCameraOffset(float duration);

    //// ポストエフェクトの再生
    //void PlayPostEffect(const std::string& effectName, float duration);

    //// フラッシュエフェクトの再生
    //void PlayFlashEffect(const XMFLOAT4& color, float duration);

private:

};

#endif // GAME_EFFECT_CONTROLLER_H