//+++++++++++++++++++++++++++++++++++++++++++++++++++
// gaussian_blur_ps.hlsl
// ・ガウシアンブラーのピクセルシェーダー
// 
// Author：Miu Kitamura
// Date  ：2026/05/06
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// 入力用構造体
struct PS_INPUT
{
    float4 posH : SV_Position;
    float2 texcoord   : TEXCOORD;
};

// ガウシアンブラー構造体
struct GaussianBlur
{
    float2 texelSize;
    float2 direction;  // ブラーの方向（例：水平なら(1,0)、垂直なら(0,1)）
    float  blur;        // ブラーの強さ（例：1.0f）
    float3 padding;
    
    float4 weights; // ブラーの重み
    float4 offsets;
};
cbuffer GaussianBlurBuffer : register(b0) {
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
        
        if (i > 0) // 中心以外は反対方向もサンプリング
        color += g_Texture.Sample(g_SamplerState, ps_in.texcoord - texOffset) * g_GaussianBlur.weights[i];
    }
    
    if (color.a <= 0.01f)
    {
       // discard;
    }
    
    return color * g_GaussianBlur.blur;
}