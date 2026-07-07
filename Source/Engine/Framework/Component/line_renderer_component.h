//---------------------------------------------------
// line_renderer_component.h
//---------------------------------------------------
#ifndef LINE_RENDERER_COMPONENT_H
#define LINE_RENDERER_COMPONENT_H

#include "Engine/Core/component.h"

#include <DirectXMath.h>
#include <vector>

class LineRendererComponent : public Component {
public:
    enum class LineType {
        LineStrip,
        LineList,
    };

    static constexpr int MAX_LINE_POINTS = 1024;

private:
    std::vector<DirectX::XMFLOAT3> m_points;
    LineType m_lineType = LineType::LineStrip;
    float m_lineWidth = 0.1f;
    DirectX::XMFLOAT4 m_lineColor = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
    LineRendererComponent()
    {
        m_points.reserve(MAX_LINE_POINTS);
    }

    void Clear()
    {
        m_points.clear();
    }

    void AddPoint(const DirectX::XMFLOAT3& point)
    {
        if (m_points.size() >= MAX_LINE_POINTS) return;
        m_points.push_back(point);
    }

    const std::vector<DirectX::XMFLOAT3>& GetPoints() const { return m_points; }
    int GetPointCount() const { return static_cast<int>(m_points.size()); }

    void SetLineType(LineType type) { m_lineType = type; }
    LineType GetLineType() const { return m_lineType; }

    void SetLineWidth(float width) { m_lineWidth = width < 0.0f ? 0.0f : width; }
    float GetLineWidth() const { return m_lineWidth; }

    void SetLineColor(const DirectX::XMFLOAT4& color) { m_lineColor = color; }
    DirectX::XMFLOAT4 GetLineColor() const { return m_lineColor; }
};

#endif // LINE_RENDERER_COMPONENT_H
