//===================================================
// bezier_line_preview_behavior.cpp
//===================================================
#include "bezier_line_preview_behavior.h"

#include "Engine/Core/game_object.h"
#include "Engine/Framework/Component/line_renderer_component.h"

#include "External/ImGui/imgui.h"

#include "Utility/mi_math.h"

using namespace DirectX;

void BezierLinePreviewBehavior::Start()
{
    m_lineRenderer = GetOwner()->GetComponent<LineRendererComponent>();
    if (!m_lineRenderer) {
        m_lineRenderer = GetOwner()->AddComponent<LineRendererComponent>();
    }

    m_lineRenderer->SetLineType(LineRendererComponent::LineType::LineStrip);
}

void BezierLinePreviewBehavior::Update()
{
    if (m_autoRebuild) {
        RebuildLine();
    }
}

void BezierLinePreviewBehavior::DrawComponentInspector()
{
    ImGui::DragInt("Sample Count", &m_sampleCount, 1.0f, 2, LineRendererComponent::MAX_LINE_POINTS);
    ImGui::Checkbox("Auto Rebuild", &m_autoRebuild);
}

void BezierLinePreviewBehavior::SetControlPoints(const XMFLOAT3 controlPoints[CONTROL_POINT_COUNT])
{
    if (!controlPoints) return;

    for (int i = 0; i < CONTROL_POINT_COUNT; ++i) {
        m_controlPoints[i] = controlPoints[i];
    }

    m_hasControlPoints = true;
    if (!m_autoRebuild) {
        RebuildLine();
    }
}

void BezierLinePreviewBehavior::SetControlPoints(
    const XMFLOAT3& p0,
    const XMFLOAT3& p1,
    const XMFLOAT3& p2,
    const XMFLOAT3& p3)
{
    m_controlPoints[0] = p0;
    m_controlPoints[1] = p1;
    m_controlPoints[2] = p2;
    m_controlPoints[3] = p3;

    m_hasControlPoints = true;
    if (!m_autoRebuild) {
        RebuildLine();
    }
}

void BezierLinePreviewBehavior::SetSampleCount(int sampleCount)
{
    if (sampleCount < 2) {
        sampleCount = 2;
    }
    if (sampleCount > LineRendererComponent::MAX_LINE_POINTS) {
        sampleCount = LineRendererComponent::MAX_LINE_POINTS;
    }

    m_sampleCount = sampleCount;
}

void BezierLinePreviewBehavior::RebuildLine()
{
    if (!m_hasControlPoints) return;

    if (!m_lineRenderer && GetOwner()) {
        m_lineRenderer = GetOwner()->GetComponent<LineRendererComponent>();
        if (!m_lineRenderer) {
            m_lineRenderer = GetOwner()->AddComponent<LineRendererComponent>();
        }
    }
    if (!m_lineRenderer) return;

    m_lineRenderer->Clear();
    m_lineRenderer->SetLineType(LineRendererComponent::LineType::LineStrip);

    const int sampleCount = m_sampleCount < 2 ? 2 : m_sampleCount;
    for (int i = 0; i < sampleCount; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(sampleCount - 1);
        m_lineRenderer->AddPoint(EvaluateBezier(t));
    }
}

void BezierLinePreviewBehavior::ClearLine()
{
    if (m_lineRenderer) {
        m_lineRenderer->Clear();
    }
}

void BezierLinePreviewBehavior::SetLineEnable(bool enable)
{
    if (m_lineRenderer) {
        m_lineRenderer->SetEnable(enable);
    }
}

XMFLOAT3 BezierLinePreviewBehavior::EvaluateBezier(float t) const
{
    t = MiMath::Clamp(t, 0.0f, 1.0f);

    const int pointCount = CONTROL_POINT_COUNT;
    const int n = pointCount - 1;

    XMFLOAT3 traj = XMFLOAT3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < pointCount; i++) {
        float bi = 1.0f;
        for (int j = 0; j < i; j++) {
            bi *= t;
        }
        for (int j = 0; j < pointCount - i - 1; j++) {
            bi *= (1.0f - t);
        }

        float coeff = 1.0f;
        for (int p = 1; p <= i; p++) {
            coeff *= static_cast<float>(n - p + 1) / static_cast<float>(p);
        }

        traj.x += coeff * bi * m_controlPoints[i].x;
        traj.y += coeff * bi * m_controlPoints[i].y;
        traj.z += coeff * bi * m_controlPoints[i].z;
    }

    return traj;
}
