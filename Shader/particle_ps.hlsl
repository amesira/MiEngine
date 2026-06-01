//+++++++++++++++++++++++++++++++++++++++++++++++++++
// particle_ps.hlsl [Particleピクセルシェーダー]
// 
// Author：Miu Kitamura
// Date  ：2026/06/01
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// 入力用頂点構造体 [Particle Vertex]
struct PS_INPUT
{
    float4 posH      : SV_Position;
    float4 color     : COLOR0;
    float2 texcoord  : TEXCOORD0;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 color = g_Texture.Sample(g_SamplerState, ps_in.texcoord) * ps_in.color;

    if (color.a < 0.01f) discard;
    return color;
}
