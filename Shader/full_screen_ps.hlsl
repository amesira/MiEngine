//+++++++++++++++++++++++++++++++++++++++++++++++++++
// full_screen_ps.hlsl
// 
// Author：Miu Kitamura
// Date  ：2026/05/19
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// 入力用構造体 [Sprite Pixel]
struct PS_INPUT
{
     float4 posH : SV_Position;
     float2 texcoord   : TEXCOORD;
};

// main関数
float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 col =  g_Texture.Sample(g_SamplerState, ps_in.texcoord);
    //if (col.a <= 0.01f) discard;
    return col;
}
