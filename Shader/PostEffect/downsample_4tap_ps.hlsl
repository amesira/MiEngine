//+++++++++++++++++++++++++++++++++++++++++++++++++++
// downsample_4tap_ps.hlsl
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

struct PS_INPUT
{
    float4 posH : SV_Position;
    float2 texcoord : TEXCOORD;
};

cbuffer Downsample4TapBuffer : register(b0)
{
    float2 g_TexelSize;
    float2 g_Padding;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float2 uv = ps_in.texcoord;

    float4 color = 0.0f;
    color += g_Texture.Sample(g_SamplerState, uv + g_TexelSize * float2(-0.5f, -0.5f));
    color += g_Texture.Sample(g_SamplerState, uv + g_TexelSize * float2( 0.5f, -0.5f));
    color += g_Texture.Sample(g_SamplerState, uv + g_TexelSize * float2(-0.5f,  0.5f));
    color += g_Texture.Sample(g_SamplerState, uv + g_TexelSize * float2( 0.5f,  0.5f));

    return color * 0.25f;
}
