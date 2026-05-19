//+++++++++++++++++++++++++++++++++++++++++++++++++++
// tone_mapping_ps.hlsl
// ・トーンマッピングのピクセルシェーダー
// 
// Author：Miu Kitamura
// Date  ：2026/05/19
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// 入力用構造体
struct PS_INPUT
{
    float4 posH : SV_Position;
    float2 texcoord   : TEXCOORD;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 color = g_Texture.Sample(g_SamplerState, ps_in.texcoord);
    if (color.a <= 0.01f)
        discard;
    
    color.rgb = color.rgb / (color.rgb + float3(1.0f, 1.0f, 1.0f)); // トーンマッピング
    return color;
}