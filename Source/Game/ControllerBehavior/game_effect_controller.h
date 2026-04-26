//---------------------------------------------------
// game_effect_controller.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/27
//---------------------------------------------------
#ifndef GAME_EFFECT_CONTROLLER_H
#define GAME_EFFECT_CONTROLLER_H
#include "Engine/Framework/Component/behavior_component.h"

class GameEffectController : public BehaviorComponent {
private:
    static inline int s_instanceCount = 0;

    // TimeScale関連パラメータ
    int     m_currentHandleID = 0;      // 現在のTimeScale変更のハンドルID
    float   m_currentTimeScale = 1.0f;  // 現在のTimeScale

    float   m_timeScaleChangeDuration = 0.0f;   // TimeScale変更の予定持続時間
    float   m_timeScaleChangeTimer = 0.0f;      // TimeScale変更の継続時間

    int     m_handleIDCounter = 0;         // 次に発行するハンドルID

public:
    GameEffectController() { s_instanceCount++; }
    ~GameEffectController() { s_instanceCount--; }
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // TimeScale変更のリクエスト処理 return : handleID
    int RequestTimeScaleChange(float targetTimeScale, float duration);
    // TimeScale変更のキャンセル処理
    void CancelTimeScaleChange(int handleID);

private:

};

#endif // GAME_EFFECT_CONTROLLER_H