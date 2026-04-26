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

class PlayerVisualMachineBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;

    // 向き更新用の変数
    float   m_currentAngleY = 0.0f;

public:
    PlayerVisualMachineBehavior() = default;
    ~PlayerVisualMachineBehavior() = default;
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // プレイヤーの戦闘状態更新処理
    void UpdateVisualMachine(PlayerContext& context, float deltaTime);

private:
    // 向き更新処理
    void UpdateFacingDirection(const PlayerContext& context, float deltaTime);
    // 傾き更新処理
    void UpdateTilt(const PlayerContext& context, float deltaTime);

};

#endif // PLAYER_VISUAL_MACHINE_BEHAVIOR_H