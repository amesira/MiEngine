//===================================================
// line_render_utility.cpp
//===================================================
#include "line_render_utility.h"

#include "Engine/Framework/Component/line_renderer_component.h"
#include "Engine/Graphics/shader_definitions.h"
#include "Engine/render_view.h"

namespace
{
    // fromベクトルからtoベクトルへの回転行列を計算する
    DirectX::XMMATRIX FromToRotation(const DirectX::XMVECTOR& from, const DirectX::XMVECTOR& to)
    {
        using namespace DirectX;

        float dot = XMVectorGetX(XMVector3Dot(from, to));
        dot = dot > 1.0f ? 1.0f : dot;
        dot = dot < -1.0f ? -1.0f : dot;

        XMVECTOR axis = XMVector3Cross(from, to);
        if (XMVectorGetX(XMVector3LengthSq(axis)) < 0.000001f) {
            return XMMatrixIdentity();
        }

        axis = XMVector3Normalize(axis);
        const float angle = acosf(dot);
        return XMMatrixRotationAxis(axis, angle);
    }

    // カメラの回転行列を計算する
    DirectX::XMMATRIX LookCameraRotation(
        const DirectX::XMMATRIX& fromToRotation,
        const DirectX::XMVECTOR& lineDirection,
        const DirectX::XMVECTOR& cameraPosition,
        const DirectX::XMVECTOR& center)
    {
        using namespace DirectX;

        const XMVECTOR up = XMVector3TransformNormal(
            XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
            fromToRotation);

        const XMVECTOR viewDirection = XMVector3Normalize(cameraPosition - center);
        XMVECTOR widthAxis = XMVector3Cross(viewDirection, lineDirection);

        if (XMVectorGetX(XMVector3LengthSq(widthAxis)) < 0.000001f) {
            widthAxis = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), lineDirection);
        }
        if (XMVectorGetX(XMVector3LengthSq(widthAxis)) < 0.000001f) {
            return XMMatrixIdentity();
        }

        widthAxis = XMVector3Normalize(widthAxis);

        const XMVECTOR cross = XMVector3Cross(up, widthAxis);
        const float sinTheta = XMVectorGetX(XMVector3Dot(cross, lineDirection));
        const float cosTheta = XMVectorGetX(XMVector3Dot(up, widthAxis));
        const float angle = atan2f(sinTheta, cosTheta);

        return XMMatrixRotationAxis(lineDirection, angle);
    }

    // lineIndexに対応する線分の両端点を取得する
    bool GetLineSegment(
        const LineRendererComponent& lineRenderer,
        int lineIndex,
        DirectX::XMFLOAT3& outP0,
        DirectX::XMFLOAT3& outP1)
    {
        const auto& points = lineRenderer.GetPoints();

        switch (lineRenderer.GetLineType()) {
        case LineRendererComponent::LineType::LineStrip:
            outP0 = points[lineIndex];
            outP1 = points[lineIndex + 1];
            return true;
        case LineRendererComponent::LineType::LineList:
            outP0 = points[lineIndex * 2];
            outP1 = points[lineIndex * 2 + 1];
            return true;
        default:
            return false;
        }
    }

    int GetLineSegmentCount(const LineRendererComponent& lineRenderer)
    {
        const int pointCount = lineRenderer.GetPointCount();
        if (pointCount < 2) return 0;

        switch (lineRenderer.GetLineType()) {
        case LineRendererComponent::LineType::LineStrip:
            return pointCount - 1;
        case LineRendererComponent::LineType::LineList:
            return pointCount / 2;
        default:
            return 0;
        }
    }
}

namespace LineRenderUtility {
    // 頂点バッファの更新
    bool UpdateLineQuadVertexBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* vertexBuffer)
    {
        if (!context || !vertexBuffer) return false;

        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        HRESULT hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return false;

        ShaderDefinitions::ParticleVertex* vertices =
            static_cast<ShaderDefinitions::ParticleVertex*>(mappedResource.pData);

        vertices[0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f);
        vertices[1].position = DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f);
        vertices[2].position = DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f);
        vertices[3].position = DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f);

        vertices[0].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
        vertices[1].texCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
        vertices[2].texCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
        vertices[3].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);

        context->Unmap(vertexBuffer, 0);
        return true;
    }

    // インスタンスバッファの更新
    int UpdateLineInstanceBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* instanceBuffer,
        const LineRendererComponent& lineRenderer,
        const RenderView& view,
        int maxInstanceCount)
    {
        using namespace DirectX;

        if (!context || !instanceBuffer || maxInstanceCount <= 0) return 0;

        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        HRESULT hr = context->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) return 0;

        ShaderDefinitions::ParticleInstanceData* instanceData =
            static_cast<ShaderDefinitions::ParticleInstanceData*>(mappedResource.pData);

        const int lineSegmentCount = GetLineSegmentCount(lineRenderer);
        const XMVECTOR cameraPosition = XMLoadFloat3(&view.eyePosition);

        int instanceCount = 0;
        for (int i = 0; i < lineSegmentCount && instanceCount < maxInstanceCount; ++i) {
            XMFLOAT3 p0 = {};
            XMFLOAT3 p1 = {};
            if (!GetLineSegment(lineRenderer, i, p0, p1)) continue;

            const XMVECTOR p0Vector = XMLoadFloat3(&p0);
            const XMVECTOR p1Vector = XMLoadFloat3(&p1);
            const XMVECTOR lineVector = p1Vector - p0Vector;
            const float lineLength = XMVectorGetX(XMVector3Length(lineVector));
            if (lineLength <= 0.0001f) continue;

            const XMVECTOR lineDirection = XMVector3Normalize(lineVector);
            const XMVECTOR centerVector = (p0Vector + p1Vector) * 0.5f;

            XMFLOAT3 center = {};
            XMStoreFloat3(&center, centerVector);

            const XMMATRIX translation = XMMatrixTranslation(center.x, center.y, center.z);
            const XMMATRIX scaling = XMMatrixScaling(lineLength, lineRenderer.GetLineWidth(), 1.0f);
            const XMMATRIX fromToRotation = FromToRotation(
                XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
                lineDirection);
            const XMMATRIX lookCameraRotation = LookCameraRotation(
                fromToRotation,
                lineDirection,
                cameraPosition,
                centerVector);

            const XMMATRIX flip = XMMatrixScaling(1.0f, -1.0f, 1.0f);

            instanceData[instanceCount].world = scaling * fromToRotation * lookCameraRotation * translation;
           // instanceData[instanceCount].world = scaling * fromToRotation * translation;
            instanceData[instanceCount].color = lineRenderer.GetLineColor();
            instanceData[instanceCount].uvRect = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

            instanceCount++;
        }

        context->Unmap(instanceBuffer, 0);
        return instanceCount;
    }
}
