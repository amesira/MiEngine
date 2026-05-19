//---------------------------------------------------
// post_process.h
// 
// 1. Tone Mapping
// 2. Bloom
// 3. Color Grading / LUT
// 4. Vignette
// 5. FXAA　→　アンチエイリアス
// 6. SSAO　→　アンビエントオクルージョン
// 
// Author：Miu Kitamura
// Date  ：2026/04/29
//---------------------------------------------------
#ifndef POST_PROCESS_H
#define POST_PROCESS_H
// direct3d
#include "Engine/Device/direct3d.h"
using namespace DirectX;
// comptr
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class ShaderProgramResource;
class ConstantBufferResource;

struct PostProcessBuffer {
    union {
        XMFLOAT4 data[8];
        struct BrighnessExtract{
            float threshold; // 輝度抽出の閾値
            float padding[3]; // パディング
        } brightnessExtract;
        struct GaussianBlur {
            XMFLOAT2 texelSize; // テクセルサイズ（1.0 / テクスチャサイズ）
            XMFLOAT2 direction; // ブラーの方向（水平: (1, 0), 垂直: (0, 1)）
            float blur;
            float padding1[3];

            XMFLOAT4 weights;
            XMFLOAT4 offsets;
        } gaussianBlur;
    };
    void Reset() {
        for (int i = 0; i < 8; i++) {
            data[i] = XMFLOAT4(0, 0, 0, 0);
        }
    }
};

class PostProcess {
private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    // フルスクリーンシェーダー
    ShaderProgramResource* m_fullScreenShader;

    // 輝度抽出用のシェーダー
    ShaderProgramResource* m_brightnessExtractShader;
    // ガウスブラー用のシェーダー
    ShaderProgramResource* m_gaussianBlurShader;
    // 4tap平均化用のシェーダー
    ShaderProgramResource* m_downsampleShader;
    // トゥーンマッピング用のシェーダー
    ShaderProgramResource* m_toneMappingShader;

    // 定数バッファ
    ConstantBufferResource* m_postProcessCB;
    PostProcessBuffer m_postProcessBufferData;

    // 一時バッファ用のテクスチャ
    ComPtr<ID3D11Texture2D> m_tempTexture[2];
    ComPtr<ID3D11RenderTargetView> m_tempRTV[2];
    ComPtr<ID3D11ShaderResourceView> m_tempSRV[2];

    // ダウンサンプリング用のテクスチャ
    enum class DownsampleLevel {
        Level1 = 0, // 1/2
        Level2,     // 1/4
        Level3,     // 1/8
        Level4,     // 1/16
        MAX,
    };
    ComPtr<ID3D11Texture2D> m_downsampledTexture[static_cast<int>(DownsampleLevel::MAX) * 2];
    ComPtr<ID3D11RenderTargetView> m_downsampledRTV[static_cast<int>(DownsampleLevel::MAX) * 2];
    ComPtr<ID3D11ShaderResourceView> m_downsampledSRV[static_cast<int>(DownsampleLevel::MAX) * 2];
    unsigned int m_downsampledWidth[static_cast<int>(DownsampleLevel::MAX)];
    unsigned int m_downsampledHeight[static_cast<int>(DownsampleLevel::MAX)];

public:
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void Finalize();
    void Process(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV);

private:
    // トーンマッピング
    void ToneMapping(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV);
    // ブルーム
    void Bloom(ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV);

    // CBの更新
    void UpdateConstantBuffer();
};

#endif // POST_PROCESS_H