//+++++++++++++++++++++++++++++++++++++++++++++++++++
// mono_mask_ps.hlsl
// ・マスク範囲以外をモノクロにするピクセルシェーダー
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

Texture2D g_MaskTexture : register(t1);

cbuffer MonoMaskBuffer : register(b0) {
    float4 g_MonoColor; // モノクロに乗算する色
    float g_Strength; // モノクロの強さ（0～1）
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 color = g_Texture.Sample(g_SamplerState, ps_in.texcoord);
    //float4 mask = g_MaskTexture.Sample(g_SamplerState, ps_in.texcoord);
    
    //// グレースケールに変換
    //float gray = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
    
    //if (mask.a < 0.5f) {
    //    // マスク範囲外はモノクロにする
    //    color.rgb = lerp(color.rgb, gray * g_MonoColor.rgb, g_Strength);
    //}
    
    return color;
}