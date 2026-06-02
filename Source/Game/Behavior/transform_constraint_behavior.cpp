//===================================================
// transform_constraint_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//===================================================
#include "transform_constraint_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Framework/Component/transform_component.h"

#include "Engine/Editor/EditorWindow/imgui_window_interface.h"

#include "Utility/mi_math.h"

void TransformConstraintBehavior::Start()
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
}

void TransformConstraintBehavior::Update()
{
    if (!m_transform || !m_target) return;
    if (!m_target->GetEnable()) return;

    m_transform->SetPosition(CalculateConstrainedPosition());
}

void TransformConstraintBehavior::DrawComponentInspector()
{
    ImGui::DragFloat3("Offset", &m_offset.x, 0.05f);
    ImGui::Checkbox("Consider Rotation", &m_considerRotation);
    ImGui::Checkbox("Consider Scaling", &m_considerScaling);
}

XMFLOAT3 TransformConstraintBehavior::CalculateConstrainedPosition() const
{
    XMFLOAT3 offset = m_offset;

    // オフセットにスケーリングを考慮する場合は、対象のスケーリングを掛ける
    if (m_considerScaling) {
        const XMFLOAT3 targetScale = m_target->GetScaling();
        offset.x *= targetScale.x;
        offset.y *= targetScale.y;
        offset.z *= targetScale.z;
    }

    const XMFLOAT3 targetPosition = m_target->GetPosition();
    if (!m_considerRotation) {
        return MiMath::Add(targetPosition, offset);
    }

    // オフセットに回転を考慮する場合は、対象の回転に合わせてオフセットを回転させる
    XMFLOAT3 rotatedOffset = { 0.0f, 0.0f, 0.0f };
    rotatedOffset = MiMath::Add(rotatedOffset, MiMath::Multiply(m_target->GetRight(), offset.x));
    rotatedOffset = MiMath::Add(rotatedOffset, MiMath::Multiply(m_target->GetUp(), offset.y));
    rotatedOffset = MiMath::Add(rotatedOffset, MiMath::Multiply(m_target->GetForward(), offset.z));

    return MiMath::Add(targetPosition, rotatedOffset);
}
