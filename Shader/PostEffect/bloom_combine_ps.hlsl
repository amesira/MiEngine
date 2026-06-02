//+++++++++++++++++++++++++++++++++++++++++++++++++++
// bloom_combine_ps.hlsl
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture0 : register(t0);
Texture2D g_Texture1 : register(t1);
Texture2D g_Texture2 : register(t2);
Texture2D g_Texture3 : register(t3);
SamplerState g_SamplerState : register(s0);

struct PS_INPUT
{
    float4 posH : SV_Position;
    float2 texcoord : TEXCOORD;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    color += g_Texture0.Sample(g_SamplerState, ps_in.texcoord);
    color += g_Texture1.Sample(g_SamplerState, ps_in.texcoord);
    color += g_Texture2.Sample(g_SamplerState, ps_in.texcoord);
    color += g_Texture3.Sample(g_SamplerState, ps_in.texcoord);
    return color * 0.25f;
}
