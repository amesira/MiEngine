//===================================================
// afterimage_behavior.cpp
//===================================================
#include "afterimage_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Device/mi_fps.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"

#include "External/ImGui/imgui.h"

#include <algorithm>

void AfterimageBehavior::Start()
{
    m_spriteRenderer = GetOwner()->GetComponent<SpriteRendererComponent>();
    if (m_spriteRenderer) {
        XMFLOAT4 color = m_spriteRenderer->GetColor();
        color.w = m_startAlpha;
        m_spriteRenderer->SetColor(color);
    }
}

void AfterimageBehavior::Update()
{
    const float deltaTime = FPS_GetDeltaTime();
    m_timer += deltaTime;

    const float t = m_lifeTime > 0.0f ? (std::min)(m_timer / m_lifeTime, 1.0f) : 1.0f;
    const float alpha = m_startAlpha + (m_endAlpha - m_startAlpha) * t;

    if (m_spriteRenderer) {
        XMFLOAT4 color = m_spriteRenderer->GetColor();
        color.w = alpha;
        m_spriteRenderer->SetColor(color);
    }

    if (t >= 1.0f && GetOwner()) {
        GetOwner()->Destroy();
    }
}

void AfterimageBehavior::DrawComponentInspector()
{
    ImGui::DragFloat("Life Time", &m_lifeTime, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Timer", &m_timer, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Start Alpha", &m_startAlpha, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("End Alpha", &m_endAlpha, 0.01f, 0.0f, 1.0f);
}

void AfterimageBehavior::Initialize(float lifeTime, float startAlpha, float endAlpha)
{
    m_lifeTime = lifeTime;
    m_startAlpha = startAlpha;
    m_endAlpha = endAlpha;
    m_timer = 0.0f;
}
