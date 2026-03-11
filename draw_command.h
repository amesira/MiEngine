// draw_command.h
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <vector>

struct DrawCommand2DInstance {
    XMFLOAT2 position;
    XMFLOAT2 size;
    float    angleZ;
    XMFLOAT4 color;
    XMFLOAT4 uvRect;
};

struct DrawBatch2D {
    int orderInLayer;
    ID3D11ShaderResourceView* texture;
    std::vector<DrawCommand2DInstance> instances;
    enum class ShaderType {
        Default,
        Font,
    } shaderType;

    DrawBatch2D() : orderInLayer(0), texture(nullptr), shaderType(ShaderType::Default) {
        instances.reserve(1024);
    }
};