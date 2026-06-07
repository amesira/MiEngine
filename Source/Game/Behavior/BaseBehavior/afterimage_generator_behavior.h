//---------------------------------------------------
// afterimage_generator_behavior.h
//---------------------------------------------------
#ifndef AFTERIMAGE_GENERATOR_BEHAVIOR_H
#define AFTERIMAGE_GENERATOR_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"

class MaterialResource;
class SpriteRendererComponent;
class TransformComponent;

class AfterimageGeneratorBehavior : public BehaviorComponent {
private:
    TransformComponent* m_sourceTransform = nullptr;
    SpriteRendererComponent* m_sourceSpriteRenderer = nullptr;

    bool m_isEmitting = false;
    float m_emitInterval = 0.04f;
    float m_emitTimer = 0.0f;
    float m_afterimageLifeTime = 0.25f;
    float m_startAlpha = 0.45f;
    float m_endAlpha = 0.0f;

    MaterialResource* m_afterimageMaterial = nullptr;

public:
    AfterimageGeneratorBehavior() = default;
    ~AfterimageGeneratorBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    void SetSource(TransformComponent* transform, SpriteRendererComponent* spriteRenderer);
    void StartEmission();
    void StopEmission();
    void EmitOnce();

    void SetEmitInterval(float interval) { m_emitInterval = interval; }
    void SetAfterimageLifeTime(float lifeTime) { m_afterimageLifeTime = lifeTime; }
    void SetAlphaRange(float startAlpha, float endAlpha);

private:
    MaterialResource* GetOrCreateAfterimageMaterial();
};

#endif // AFTERIMAGE_GENERATOR_BEHAVIOR_H
