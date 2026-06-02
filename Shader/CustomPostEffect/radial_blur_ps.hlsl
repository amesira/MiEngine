//+++++++++++++++++++++++++++++++++++++++++++++++++++
// radial_blur_ps.hlsl
// ・ラディアルブラーのピクセルシェーダー
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// 入力用構造体
struct PS_INPUT
{
    float4 posH : SV_Position;
    float2 texcoord   : TEXCOORD;
};

cbuffer RadialBlurBuffer : register(b0) {
    int     g_SampleCount;  // サンプル数
    float   g_Strength;     // ブラーの強さ
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // UVを-0.5~0.5に変換
    float2 symmertryUv = ps_in.texcoord - float2(0.5f, 0.5f);
    
    // 外側に行くほどこの値が大きくなる(0～0.707)
    float distance = length(symmertryUv);

    for(int j = 0; j < g_SampleCount; j++) {
        // jが大きいほど、画面の外側ほど小さくなる値
        float uvOffset = 1 - g_Strength * j / g_SampleCount * distance;

        // jが大きくなるにつれてより内側のピクセルをサンプリングしていく
        // また画面の外側ほどより内側のピクセルをサンプリングする
        color += g_Texture.Sample(g_SamplerState, symmertryUv * uvOffset + float2(0.5f, 0.5f));
    }

    color /= g_SampleCount;
    
    return color;
}