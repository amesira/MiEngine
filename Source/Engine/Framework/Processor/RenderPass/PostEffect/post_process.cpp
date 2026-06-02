//===================================================
// post_process.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//===================================================
#include "post_process.h"

#include "Engine/engine_service_locator.h"
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

void PostProcess::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_device = device;
    m_context = context;

    unsigned int SCREEN_WIDTH = Direct3D_GetBackBufferWidth();
    unsigned int SCREEN_HEIGHT = Direct3D_GetBackBufferHeight();

    // 一時バッファ用のテクスチャとビューの作成
    for (int i = 0; i < 2; i++) {
        Direct3D_CreateColorBuffer(
            m_tempTexture[i].GetAddressOf(),
            m_tempRTV[i].GetAddressOf(), 
            m_tempSRV[i].GetAddressOf(),
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        );
    }

    // ダウンサンプリング用のテクスチャとビューの作成
    for (int level = 0; level < static_cast<int>(DownsampleLevel::MAX); level++) {
        m_downsampledWidth[level] = SCREEN_WIDTH >> (level + 1); // 1/2, 1/4, 1/8, 1/16
        m_downsampledHeight[level] = SCREEN_HEIGHT >> (level + 1);

        for (int i = 0; i < 2; i++) {
            Direct3D_CreateColorBuffer(
                m_downsampledTexture[level * 2 + i].GetAddressOf(),
                m_downsampledRTV[level * 2 + i].GetAddressOf(),
                m_downsampledSRV[level * 2 + i].GetAddressOf(),
                m_downsampledWidth[level],
                m_downsampledHeight[level]
            );
        }
    }

    // 定数バッファの作成
    m_postProcessCB = SHADER_REPOSITORY->GenerateConstantBufferResource(
        "PostProcessBuffer",
        0,
        sizeof(PostProcessBuffer),
        true,
        true,
        ConstantBufferUsage::Dynamic);
    SHADER_REPOSITORY->AddConstantBufferToShaderProgram(SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::FullScreen)], m_postProcessCB);

    // シェーダーのロード
    m_fullScreenShader = SHADER_REPOSITORY->GetShaderProgramResource(ShaderBase::FullScreen);

    ShaderProgramResource brightnessExtractShaderResource;
    brightnessExtractShaderResource.name = "BrightnessExtract";
    brightnessExtractShaderResource.baseShader = m_fullScreenShader;
    brightnessExtractShaderResource.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource("brightness_extract_ps.cso");
    m_brightnessExtractShader = SHADER_REPOSITORY->GenerateShaderProgramResource(brightnessExtractShaderResource);

    ShaderProgramResource gaussianBlurShaderResource;
    gaussianBlurShaderResource.name = "GaussianBlur";
    gaussianBlurShaderResource.baseShader = m_fullScreenShader;
    gaussianBlurShaderResource.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource("gaussian_blur_ps.cso");
    m_gaussianBlurShader = SHADER_REPOSITORY->GenerateShaderProgramResource(gaussianBlurShaderResource);

    ShaderProgramResource downsampleShaderResource;
    downsampleShaderResource.name = "Downsample";
    downsampleShaderResource.baseShader = m_fullScreenShader;
    downsampleShaderResource.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource("downsample_4_ps.cso");
    m_downsampleShader = SHADER_REPOSITORY->GenerateShaderProgramResource(downsampleShaderResource);

    ShaderProgramResource toneMappingShaderResource;
    toneMappingShaderResource.name = "ToneMapping";
    toneMappingShaderResource.baseShader = m_fullScreenShader;
    toneMappingShaderResource.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource("tone_mapping_ps.cso");
    m_toneMappingShader = SHADER_REPOSITORY->GenerateShaderProgramResource(toneMappingShaderResource);
    
}

void PostProcess::Finalize()
{
    // リソースはComPtrが自動で解放するため、特に処理は不要
}

void PostProcess::Process(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV)
{
    Direct3D_ClearSceneTarget(outputRTV, nullptr, 1.0f);

    // ブルーム
    Bloom(inputSRV, outputRTV);
}

// -------------------------- private

// トーンマッピング
void PostProcess::ToneMapping(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV)
{
    EngineServiceLocator::BindShader(m_toneMappingShader);
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);
    m_context->PSSetShaderResources(0, 1, &inputSRV);

    Direct3D_ResetViewport();
    Direct3D_ClearSceneTarget(outputRTV, nullptr);
    Direct3D_SetSceneTarget(outputRTV, nullptr);

    m_context->Draw(3, 0); // フルスクリーン三角形を描画
}

// ブルーム
void PostProcess::Bloom(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV)
{
    // 1.輝度抽出
    Direct3D_ResetViewport();
    Direct3D_ClearSceneTarget(m_tempRTV[0].Get(), nullptr, 1.0f);
    Direct3D_SetSceneTarget(m_tempRTV[0].Get(), nullptr);
    EngineServiceLocator::BindShader(m_brightnessExtractShader);

    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);
    m_context->PSSetShaderResources(0, 1, &inputSRV);

    m_postProcessBufferData.Reset();
    {
        auto& be = m_postProcessBufferData.brightnessExtract;
        be.threshold = 2.0f; // 輝度抽出の閾値
    }
    UpdateConstantBuffer();
    
    m_context->Draw(3, 0); // フルスクリーン三角形を描画

    // 2.ガウスブラー
    EngineServiceLocator::BindShader(m_gaussianBlurShader);
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    m_postProcessBufferData.Reset();
    {
        auto& gb = m_postProcessBufferData.gaussianBlur;
        gb.blur = 1.0f;
        gb.weights = XMFLOAT4(
            0.227027f,
            0.1945946f,
            0.1216216f,
            0.054054f); // ガウスカーネルの重み
        gb.offsets = XMFLOAT4(0, 1, 2, 3); // サンプルオフセット
    }

    for (int i = 0; i < static_cast<int>(DownsampleLevel::MAX); i++) {
        auto& gb = m_postProcessBufferData.gaussianBlur;
        gb.texelSize = XMFLOAT2(1.0f / m_downsampledWidth[i], 1.0f / m_downsampledHeight[i]);

        // 水平方向のブラー
        Direct3D_SetViewport(m_downsampledWidth[i], m_downsampledHeight[i]);
        Direct3D_ClearSceneTarget(m_downsampledRTV[i * 2].Get(), nullptr, 1.0f);
        Direct3D_SetSceneTarget(m_downsampledRTV[i * 2].Get(), nullptr);
        
        gb.direction = XMFLOAT2(1, 0);
        UpdateConstantBuffer();
        
        if (i == 0) {
            m_context->PSSetShaderResources(0, 1, m_tempSRV[0].GetAddressOf());
        } else {
            m_context->PSSetShaderResources(0, 1, m_downsampledSRV[(i - 1) * 2].GetAddressOf());
        }

        m_context->Draw(3, 0); // フルスクリーン三角形を描画

        // 垂直方向のブラー
        Direct3D_ClearSceneTarget(m_downsampledRTV[i * 2 + 1].Get(), nullptr, 1.0f);
        Direct3D_SetSceneTarget(m_downsampledRTV[i * 2 + 1].Get(), nullptr);

        gb.direction = XMFLOAT2(0, 1);
        UpdateConstantBuffer();

        m_context->PSSetShaderResources(0, 1, m_downsampledSRV[i * 2].GetAddressOf());

        m_context->Draw(3, 0); // フルスクリーン三角形を描画
    }

    // 3.ブラー結果の合成
    Direct3D_ResetViewport();
    Direct3D_ClearSceneTarget(m_tempRTV[1].Get(), nullptr, 1.0f);
    Direct3D_SetSceneTarget(m_tempRTV[1].Get(),nullptr);

    EngineServiceLocator::BindShader(m_downsampleShader);
    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);

    for (int i = 0; i < static_cast<int>(DownsampleLevel::MAX); i++) {
        m_context->PSSetShaderResources(i, 1, m_downsampledSRV[i * 2 + 1].GetAddressOf());
    }
    m_context->Draw(3, 0); // フルスクリーン三角形を描画

    // 4.元のシーンとブラー結果の合成
    Direct3D_ResetViewport();
    Direct3D_SetSceneTarget(outputRTV, nullptr);
    EngineServiceLocator::BindShader(m_fullScreenShader);

    SetBlendState(BLENDSTATE_NONE);
    SetDepthState(DEPTHSTATE_DISABLE);
    m_context->PSSetShaderResources(0, 1, &inputSRV);
    m_context->Draw(3, 0); // フルスクリーン三角形を描画
    
    SetBlendState(BLENDSTATE_ADD);
    SetDepthState(DEPTHSTATE_DISABLE);
    m_context->PSSetShaderResources(0, 1, m_tempSRV[1].GetAddressOf());
    m_context->Draw(3, 0); // フルスクリーン三角形を描画

    // テクスチャアンバインド処理
    m_context->PSSetShaderResources(0, 1, nullptr);
    m_context->PSSetShaderResources(1, 1, nullptr);
    m_context->PSSetShaderResources(2, 1, nullptr);
    m_context->PSSetShaderResources(3, 1, nullptr);
}

// CBの更新
void PostProcess::UpdateConstantBuffer()
{
    D3D11_MAPPED_SUBRESOURCE msr = {};
    m_context->Map(m_postProcessCB->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    XMFLOAT4* cbData = reinterpret_cast<XMFLOAT4*>(msr.pData);
    for (int i = 0; i < 8; i++) {
        cbData[i] = m_postProcessBufferData.data[i];
    }
    m_context->Unmap(m_postProcessCB->buffer.Get(), 0);
}
