//---------------------------------------------------
// bezier_line_preview_behavior.h
//---------------------------------------------------
#ifndef BEZIER_LINE_PREVIEW_BEHAVIOR_H
#define BEZIER_LINE_PREVIEW_BEHAVIOR_H

#include "Engine/Framework/Component/behavior_component.h"

#include <DirectXMath.h>

class LineRendererComponent;

class BezierLinePreviewBehavior : public BehaviorComponent {
private:
    static constexpr int CONTROL_POINT_COUNT = 4;

    LineRendererComponent* m_lineRenderer = nullptr;
    DirectX::XMFLOAT3 m_controlPoints[CONTROL_POINT_COUNT] = {};
    int m_sampleCount = 24;
    bool m_autoRebuild = true;
    bool m_hasControlPoints = false;

public:
    BezierLinePreviewBehavior() = default;
    ~BezierLinePreviewBehavior() = default;

    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    void SetControlPoints(const DirectX::XMFLOAT3 controlPoints[CONTROL_POINT_COUNT]);
    void SetControlPoints(
        const DirectX::XMFLOAT3& p0,
        const DirectX::XMFLOAT3& p1,
        const DirectX::XMFLOAT3& p2,
        const DirectX::XMFLOAT3& p3);

    void SetSampleCount(int sampleCount);
    int GetSampleCount() const { return m_sampleCount; }

    void SetAutoRebuild(bool autoRebuild) { m_autoRebuild = autoRebuild; }
    bool GetAutoRebuild() const { return m_autoRebuild; }

    void RebuildLine();
    void ClearLine();

private:
    DirectX::XMFLOAT3 EvaluateBezier(float t) const;
};

#endif // BEZIER_LINE_PREVIEW_BEHAVIOR_H
