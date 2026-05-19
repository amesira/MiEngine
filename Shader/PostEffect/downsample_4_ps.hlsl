//+++++++++++++++++++++++++++++++++++++++++++++++++++
// downsample_4_ps.hlsl
// ・4つのピクセルを平均してダウンサンプリングするピクセルシェーダー
// 
// Author：Miu Kitamura
// Date  ：2026/05/19
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture0 : register(t0);
Texture2D g_Texture1 : register(t1);
Texture2D g_Texture2 : register(t2);
Texture2D g_Texture3 : register(t3);
SamplerState g_SamplerState : register(s0);

// 入力用構造体
struct PS_INPUT
{
    float4 posH : SV_Position;
    float2 texcoord   : TEXCOORD;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    color += g_Texture0.Sample(g_SamplerState, ps_in.texcoord);
    color += g_Texture1.Sample(g_SamplerState, ps_in.texcoord);
    color += g_Texture2.Sample(g_SamplerState, ps_in.texcoord);
    color += g_Texture3.Sample(g_SamplerState, ps_in.texcoord);
    color *= 0.25f; // 4つのピクセルの平均を取る
   
    if (color.a <= 0.01f)
    {
        //discard;
    }
    
    return color;
}