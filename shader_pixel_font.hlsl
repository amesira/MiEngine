//===================================================
// shader_pixel_font.hlsl
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

struct PS_INPUT // VS_OUTPUTと同じ内容
{
    float4 posH : SV_Position;   // ピクセルの座標
    float4 posW : POSITION1; // ワールド座標
    float4 normal : NORMAL0; // ピクセルの法線
    float4 color : COLOR0;       // ピクセルの色
    float2 texcoord : TEXCOORD0; // テクスチャ座標
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 col;
	
    col = float4(1.0, 1.0, 1.0, g_Texture.Sample(g_SamplerState, ps_in.texcoord).r);
	
    col *= ps_in.color;
    if (col.a < 0.01)
    {
        discard;
    }
    return col;
}