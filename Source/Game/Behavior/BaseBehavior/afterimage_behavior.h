//---------------------------------------------------
// afterimage_behavior.h
//---------------------------------------------------
#ifndef AFTERIMAGE_BEHAVIOR_H
#define AFTERIMAGE_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"

class SpriteRendererComponent;

class AfterimageBehavior : public BehaviorComponent {
private:
    SpriteRendererComponent* m_spriteRenderer = nullptr;

    float m_lifeTime = 0.25f;
    float m_timer = 0.0f;
    float m_startAlpha = 0.45f;
    float m_endAlpha = 0.0f;

public:
    AfterimageBehavior() = default;
    ~AfterimageBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    void Initialize(float lifeTime, float startAlpha, float endAlpha = 0.0f);
};

#endif // AFTERIMAGE_BEHAVIOR_H
