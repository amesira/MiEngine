//+++++++++++++++++++++++++++++++++++++++++++++++++++
// gaussian_blur_ps.hlsl
// ・ガウシアンブラーのピクセルシェーダー
// 
// Author：Miu Kitamura
// Date  ：2026/05/06
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// 入力用構造体（Unlitと同様）
struct PS_INPUT
{
    float4 posH     : SV_Position;  // 変換済み頂点座標
    float4 color    : COLOR0;       // 頂点カラー
    float2 texcoord : TEXCOORD0;    // テクスチャ座標
};

// ガウシアンブラー構造体
struct GaussianBlur
{
    float2 texelSize;
    float2 direction;  // ブラーの方向（例：水平なら(1,0)、垂直なら(0,1)）
    float  blur;        // ブラーの強さ（例：1.0f）
    float3 padding;
    
    float4 weights;    // ブラーの重み
    float4 offsets;    // ブラーのオフセット
};
cbuffer GaussianBlurBuffer : register(b1) {
    GaussianBlur g_GaussianBlur;
}

// main関数
float4 main(PS_INPUT ps_in) : SV_TARGET
{
    // 中心は0.0fとする
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float2 texOffset = float2(0.0f, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        texOffset = g_GaussianBlur.direction * g_GaussianBlur.texelSize * g_GaussianBlur.offsets[i];
        
        // テクスチャからサンプリングして、重みを掛けて加算
        color += g_Texture.Sample(g_SamplerState, ps_in.texcoord + texOffset) * g_GaussianBlur.weights[i];
        color += g_Texture.Sample(g_SamplerState, ps_in.texcoord - texOffset) * g_GaussianBlur.weights[i];
    }
    
    return color * g_GaussianBlur.blur;
}