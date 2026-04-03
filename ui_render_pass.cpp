//===================================================
// ui_render_pass.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//===================================================
#include "ui_render_pass.h"

#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

#include <algorithm>

#include "engine_service_locator.h"

// UiRenderPassの初期化
void UIRenderPass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_batches.reserve(2048);

    // 描画コマンドの収集クラスの初期化
    m_collectorImage.Initialize();
    m_collectorSlider.Initialize();
    m_collectorFont.Initialize();

    // 頂点バッファ生成
    {
	    D3D11_BUFFER_DESC bd = {};
	    bd.Usage = D3D11_USAGE_DYNAMIC;
	    bd.ByteWidth = sizeof(LitVertex) * 4;
	    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	    m_pDevice->CreateBuffer(&bd, NULL, &m_pVertexBuffer);
    }
}

// UiRenderPassの終了処理
void UIRenderPass::Finalize()
{
    // 描画コマンドの収集クラスの終了処理
    m_collectorImage.Finalize();
    m_collectorSlider.Finalize();
    m_collectorFont.Finalize();
}

// UiRenderPassの処理
void UIRenderPass::Process(IScene* pScene)
{
    //----------------------------------------------------
    // UI描画のセットアップ
	//----------------------------------------------------
    SetBlendState(BLENDSTATE_ALFA);
    SetDepthState(DEPTHSTATE_DISABLE);

    // UI描画コマンドのバッチ処理
    m_batches.clear();
    m_collectorImage.CollectDrawBatches2D(pScene, m_batches);
    m_collectorSlider.CollectDrawBatches2D(pScene, m_batches);
    m_collectorFont.CollectDrawBatches2D(pScene, m_batches);

    // ソート（レイヤー順）
    std::sort(m_batches.begin(), m_batches.end(), [](const DrawBatch2D& a, const DrawBatch2D& b) {
        return a.orderInLayer < b.orderInLayer;
        });

    //----------------------------------------------------
    // UI描画コマンドの実行
    //----------------------------------------------------
    int currentShaderType = -1;
    for (const DrawBatch2D& batch : m_batches) {

        // シェーダーの切り替えが必要なら切り替える
        if (batch.shaderType != static_cast<DrawBatch2D::ShaderType>(currentShaderType)) {
            // シェーダーの切り替え
            switch (batch.shaderType) {
            case DrawBatch2D::ShaderType::Default:
                EngineServiceLocator::BindShader(ShaderManager::ShaderType::Unlit);
                break;
            case DrawBatch2D::ShaderType::Font:
                EngineServiceLocator::BindShader(ShaderManager::ShaderType::TlueTypeFontUnlit);
                break;
            }
            currentShaderType = static_cast<int>(batch.shaderType);
        }

        // テクスチャのセット
        m_pContext->PSSetShaderResources(0, 1, &batch.texture);

        for (const DrawCommand2DInstance& instance : batch.instances) {

            // ワールド行列の計算
            XMMATRIX world = XMMatrixIdentity();
            {
                XMMATRIX translation = XMMatrixTranslation(instance.position.x, instance.position.y, 0.0f);
                XMMATRIX scale = XMMatrixScaling(instance.size.x, instance.size.y, 1.0f);
                XMMATRIX rotation = XMMatrixRotationZ(instance.angleZ);

                world = scale * rotation * translation;
            }

            // シェーダーの定数バッファにワールド行列を更新
            EngineServiceLocator::UpdateTransformCB({ world, XMMatrixIdentity() });

            // 頂点バッファに頂点データを転送
            {
                D3D11_MAPPED_SUBRESOURCE msr;
                m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
                LitVertex* v = (LitVertex*)msr.pData;

                v[0].position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
                v[0].texCoord = XMFLOAT2(instance.uvRect.x, instance.uvRect.y);

                v[1].position = XMFLOAT3(0.5f, -0.5f, 0.0f);
                v[1].texCoord = XMFLOAT2(instance.uvRect.x + instance.uvRect.z, instance.uvRect.y);

                v[2].position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
                v[2].texCoord = XMFLOAT2(instance.uvRect.x, instance.uvRect.y + instance.uvRect.w);

                v[3].position = XMFLOAT3(0.5f, 0.5f, 0.0f);
                v[3].texCoord = XMFLOAT2(instance.uvRect.x + instance.uvRect.z, instance.uvRect.y + instance.uvRect.w);

                for (int i = 0; i < 4; i++) {
                    v[i].color = instance.color;
                    v[i].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
                }
                m_pContext->Unmap(m_pVertexBuffer, 0);
            }

            // 頂点バッファの設定
            UINT stride = sizeof(LitVertex);
            UINT offset = 0;
            m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

            // プリミティブトポロジ設定 トライアングルストリップ
            m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

            // ポリゴン描画命令発行
            m_pContext->Draw(4, 0);
        }
    }
}