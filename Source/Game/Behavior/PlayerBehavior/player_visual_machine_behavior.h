//---------------------------------------------------
// player_visual_machine_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/26
//---------------------------------------------------
#ifndef PLAYER_VISUAL_MACHINE_BEHAVIOR_H
#define PLAYER_VISUAL_MACHINE_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "player_context.h"

class TransformComponent;
class AnimationComponent;

class PlayerVisualMachineBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;
    AnimationComponent* m_animation = nullptr;

    // 傾きの最大値（ラジアン）
    XMFLOAT3 m_forwardTilt = { XMConvertToRadians(15.0f), 0.0f, 0.0f};
    XMFLOAT3 m_rightTilt = { 0.0f, 0.0f, XMConvertToRadians(10.0f) };

    // スローモーションの状態
    bool m_isInSlowMotion = false;
    float m_slowMotionTimer = 0.0f;
    float m_slowMotionDuration = 0.0f;
    float m_slowMotionFactor = 0.5f;

public:
    PlayerVisualMachineBehavior() = default;
    ~PlayerVisualMachineBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // プレイヤーの戦闘状態更新処理
    void UpdateVisualMachine(PlayerContext& context, float deltaTime);

    // スローモーションのトリガー
    void TriggerSlowMotion(SlowMotionRequest& request);
    // エフェクトのトリガー
    void TriggerEffect(EffectRequest& request);
    // ポストエフェクト（色相）のトリガー
    void TriggerPostEffectColor(PostEffectColorRequest& request);
    // ポストエフェクト（画面効果）のトリガー
    void TriggerPostEffectScreen(PostEffectScreenRequest& request);

private:
    // 傾き更新処理
    void UpdateTilt(const PlayerContext& context, float deltaTime);

    // スローモーション更新処理
    bool UpdateSlowMotion(float deltaTime);

};

#endif // PLAYER_VISUAL_MACHINE_BEHAVIOR_H