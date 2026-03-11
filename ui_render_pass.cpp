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

#include "sprite.h"
#include "shader.h"

#include <algorithm>

// UiRenderPassの初期化
void UIRenderPass::Initialize()
{
    m_batches.reserve(2048);

    // 描画コマンドの収集クラスの初期化
    m_collectorImage.Initialize();
    m_collectorSlider.Initialize();
    m_collectorFont.Initialize();
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
    Shader_Begin();
    SetBlendState(BLENDSTATE_ALFA);

    const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
    const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
    Shader_SetMatrix(XMMatrixOrthographicOffCenterLH(
        0.0f,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0.0f,
        0.0f,
        1.0f));

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
    DrawBatch2D::ShaderType currentShaderType = DrawBatch2D::ShaderType::Default;
    for (const DrawBatch2D& batch : m_batches) {
        // シェーダーの切り替えが必要なら切り替える
        if (batch.shaderType != currentShaderType) {
            // シェーダーの切り替え
            switch (batch.shaderType) {
            case DrawBatch2D::ShaderType::Default:
                Shader_Begin();
                break;
            case DrawBatch2D::ShaderType::Font:
                Shader_Begin(ShaderBeginMode::TrueTypeFont);
                break;
            }
            currentShaderType = batch.shaderType;
        }

        // インスタンシング準備
        PrepareDrawInstance();

        // テクスチャをシェーダーに設定
        SetTexture(batch.texture);

        for (const DrawCommand2DInstance& instance : batch.instances) {
            // ワールド行列の計算
            XMMATRIX world = XMMatrixIdentity();
            {
                XMMATRIX translation = XMMatrixTranslation(instance.position.x, instance.position.y, 0.0f);
                XMMATRIX scale = XMMatrixScaling(instance.size.x, instance.size.y, 1.0f);
                XMMATRIX rotation = XMMatrixRotationZ(instance.angleZ);
                world = scale * rotation * translation;
            }

            // インスタンシングデータの追加
            AddInstanceData(world, instance.color, instance.uvRect);
        }

        // インスタンシング描画命令
        DrawInstance();
    }

    Shader_Begin();
    ClearInstanceData();
}