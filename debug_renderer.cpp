//===================================================
// debug_renderer.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/04
//===================================================
#include "debug_renderer.h"
#include <vector>
#include "shader.h"
#include "sprite.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static constexpr int NUM_VERTEX = 10000 * 2; // 最大頂点数（ライン10000本分）
static ID3D11Buffer* g_pLineVertexBuffer = nullptr;
static std::vector<Vertex> g_LineVertices;

void DebugRenderer_Initialize()
{
    g_pDevice = Direct3D_GetDevice();
    g_pContext = Direct3D_GetDeviceContext();

    // ライン頂点バッファ生成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX; // 格納する最大頂点数
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    g_pDevice->CreateBuffer(&bd, NULL, &g_pLineVertexBuffer);

    // ライン頂点バッファの初期容量を設定
    g_LineVertices.reserve(NUM_VERTEX);
}

void DebugRenderer_Finalize()
{
    SAFE_RELEASE(g_pLineVertexBuffer);
}

void DebugRenderer_DrawFlush()
{
    if(g_LineVertices.empty()) return;

    Shader_Begin();

    XMMATRIX projection = Direct3D_GetProjectionMatrix();
    XMMATRIX view = Direct3D_GetViewMatrix();

    Shader_SetMatrix(view * projection);

    //----------------------------------------------------
    // 頂点バッファを更新
	//----------------------------------------------------
    {
        // 頂点バッファをロック
        D3D11_MAPPED_SUBRESOURCE msr;
        g_pContext->Map(g_pLineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

        // 頂点バッファへの仮想ポインタを取得
        Vertex* v = (Vertex*)msr.pData;

        for(int i = 0; i < g_LineVertices.size(); i++) {
            v[i].position = g_LineVertices[i].position;
            v[i].color = g_LineVertices[i].color;
        }

        // 頂点バッファのロックを解除
        g_pContext->Unmap(g_pLineVertexBuffer, 0);
    }

    // 頂点バッファを描画パイプラインに設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_pContext->IASetVertexBuffers(0, 1, &g_pLineVertexBuffer, &stride, &offset);

    // プリミティブトポロジ設定 トライアングルストリップ
    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // ポリゴン描画命令発行
    g_pContext->Draw(g_LineVertices.size(), 0);

    
}

void DebugRenderer_ResetBuffer()
{
    g_LineVertices.clear();
}

void DrawLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color)
{
    // 頂点バッファの容量オーバー防止
    if(g_LineVertices.size() + 2 > NUM_VERTEX) {
        return;
    }

    // ライン頂点をバッファに追加
    g_LineVertices.push_back({start, {0.0f,0.0f,0.0f},color, {0.0f, 0.0f}});
    g_LineVertices.push_back({ end, {0.0f,0.0f,0.0f},color, {0.0f, 0.0f} });
}
