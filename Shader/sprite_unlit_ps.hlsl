//+++++++++++++++++++++++++++++++++++++++++++++++++++
// sprite_unlit_ps.hlsl
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

struct PS_INPUT
{
    float4 posH     : SV_Position;
    float4 posW     : POSITION1;
    float4 normal   : NORMAL0;
    float4 color    : COLOR0;
    float2 texcoord : TEXCOORD0;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 col = ps_in.color;
    col *= g_Texture.Sample(g_SamplerState, ps_in.texcoord);

    if (col.a <= 0.01f) discard;
    return col;
}
