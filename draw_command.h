// draw_command.h
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <vector>

// 2D描画コマンドのインスタンス情報
struct DrawCommand2DInstance {
    XMFLOAT2 position;
    XMFLOAT2 size;
    float    angleZ;
    XMFLOAT4 color;
    XMFLOAT4 uvRect;
};

// 2D描画コマンドのバッチ情報
struct DrawBatch2D {
    // レイヤー内の描画順序
    int orderInLayer;
    // 描画に使用するテクスチャ
    ID3D11ShaderResourceView* texture;
    // 使用するシェーダーの種類
    enum class ShaderType {
        Default,
        Font,
    } shaderType;

    // 描画コマンドのインスタンス情報のリスト
    std::vector<DrawCommand2DInstance> instances;
    
    DrawBatch2D() : orderInLayer(0), texture(nullptr), shaderType(ShaderType::Default) {
        instances.reserve(1024);
    }
};