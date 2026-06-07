//---------------------------------------------------
// shake_object_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/07
//---------------------------------------------------
#ifndef SHAKE_OBJECT_BEHAVIOR_H
#define SHAKE_OBJECT_BEHAVIOR_H

#include "Engine/Core/GamePlay/sequence_task.h"
#include "Engine/Framework/Component/behavior_component.h"

#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;

class ShakeObjectBehavior : public BehaviorComponent {
private:
    // シーケンスタスクを継承した、揺れのタスクとオフセットリセットのタスク
    class ShakeTask : public SequenceTask {
    public:
        float m_duration = 0.0f;
        float m_magnitude = 0.0f;
        float m_frequency = 35.0f;
        XMFLOAT3 m_currentOffset = { 0.0f, 0.0f, 0.0f };

        void Update(float deltaTime) override;
    };
    class ResetOffsetTask : public SequenceTask {
    public:
        float m_duration = 0.0f;
        XMFLOAT3 m_startOffset = { 0.0f, 0.0f, 0.0f };
        XMFLOAT3 m_currentOffset = { 0.0f, 0.0f, 0.0f };

        void Start() override;
        void Update(float deltaTime) override;
    };

    TransformComponent* m_transform = nullptr;

    // 揺れのタスク
    ShakeTask m_shakeTask;
    // オフセットリセットのタスク
    ResetOffsetTask m_resetOffsetTask;

    XMFLOAT3 m_previousOffset = { 0.0f, 0.0f, 0.0f };
    bool m_hasAppliedOffset = false;

public:
    ShakeObjectBehavior() = default;
    ~ShakeObjectBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // シェイクタスク
    void Shake(float duration, float magnitude, float frequency = 35.0f);
    void ShakeTemporary(float duration, float magnitude, float frequency = 35.0f);
    void Reset(float duration = 0.0f);

    // シェイク中かどうか
    bool IsShaking();

private:
    XMFLOAT3 GetBasePosition() const;
    void ApplyOffset(const XMFLOAT3& basePosition, const XMFLOAT3& offset);
    void ClearOffset();
};

#endif // SHAKE_OBJECT_BEHAVIOR_H
