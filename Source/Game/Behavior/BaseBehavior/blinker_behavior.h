//---------------------------------------------------
// blinker_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/07
//---------------------------------------------------
#ifndef BLINKER_BEHAVIOR_H
#define BLINKER_BEHAVIOR_H

#include "Engine/Core/GamePlay/tween_task.h"
#include "Engine/Framework/Component/behavior_component.h"

#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

class MaterialInstance;
class ModelComponent;
class SpriteRendererComponent;

class BlinkerBehavior : public BehaviorComponent {
private:
    enum class MaterialTargetType {
        Model,
        Sprite,
    };

    // ブリンクのモード
    enum class BlinkMode {
        None,
        Flash,
        FlashTemporary,
        Reset,
    };

    struct EmissionState {
        bool isOverride = false;
        XMFLOAT3 color = { 0.0f, 0.0f, 0.0f };
        float intensity = 0.0f;
    };

    struct MaterialTarget {
        MaterialTargetType type = MaterialTargetType::Model;
        size_t slotIndex = 0;
        EmissionState defaultState;
        EmissionState startState;
    };

    ModelComponent* m_model = nullptr;
    SpriteRendererComponent* m_spriteRenderer = nullptr;

    std::vector<MaterialTarget> m_targets;

    BlinkMode m_mode = BlinkMode::None;
    FloatTweenTask m_emissionRateTask;
    XMFLOAT3 m_flashColor = { 1.0f, 1.0f, 1.0f };
    float m_flashIntensity = 1.0f;

public:
    BlinkerBehavior() = default;
    ~BlinkerBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    void Flash(const XMFLOAT3& color, float intensity, float duration = 0.0f);
    void FlashTemporary(const XMFLOAT3& color, float intensity, float duration, float holdDuration);
    void Reset(float duration = 0.0f);

    bool IsFlashing() const { return m_mode != BlinkMode::None; }

private:
    void RefreshTargets();
    void CaptureStartStates();

    MaterialInstance* ResolveMaterial(const MaterialTarget& target);
    EmissionState GetEmissionState(MaterialInstance* material) const;
    void ApplyEmissionState(MaterialInstance* material, const EmissionState& state);
    void ApplyFlash(float rate);
    void ApplyReset(float rate);
    void RestoreDefaultStates();
};

#endif // BLINKER_BEHAVIOR_H
