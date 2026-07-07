//---------------------------------------------------
// line_render_utility.h
//---------------------------------------------------
#ifndef LINE_RENDER_UTILITY_H
#define LINE_RENDER_UTILITY_H

#include "Engine/Device/direct3d.h"

class LineRendererComponent;
struct RenderView;

namespace LineRenderUtility {
    bool UpdateLineQuadVertexBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* vertexBuffer);

    int UpdateLineInstanceBuffer(
        ID3D11DeviceContext* context,
        ID3D11Buffer* instanceBuffer,
        const LineRendererComponent& lineRenderer,
        const RenderView& view,
        int maxInstanceCount);
}

#endif // LINE_RENDER_UTILITY_H
