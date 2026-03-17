//+++++++++++++++++++++++++++++++++++++++++++++++++++
// shader_pixel.hlsl
// 
// Author：Miu Kitamura
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "shader_light.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

cbuffer Option : register(b0)
{
    float4 colorRate;
    float4 alphaColor;
    float  grayRate; // 0.0 = 通常, 1.0 = 完全グレー
};

struct PS_INPUT // VS_OUTPUTと同じ内容
{
    float4 posH     : SV_Position;  // ピクセルの座標
    float4 posW : POSITION1; // ワールド座標
    float4 normal   : NORMAL0;      // ピクセルの法線
    float4 color    : COLOR0;       // ピクセルの色
    float2 texcoord : TEXCOORD0;    // テクスチャ座標
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 col;
    
    // テクスチャの色を取得・乗算
    col = g_Texture.Sample(g_SamplerState, ps_in.texcoord);
    col *= ps_in.color;
    
    // ライトの影響を加算
    if (g_EnableLighting != 0)
    {
        col.rgb *= CalcLight_DirectionalLights(ps_in.normal.xyz);
        col.rgb += CalcLight_PointLights(ps_in.posW.xyz, ps_in.normal.xyz) * 3.0f;
        col.rgb += CalcLight_SpotLights(ps_in.posW.xyz, ps_in.normal.xyz);
    }
    
    col.rgb *= colorRate.rgb; // 色の乗算
    
    if (col.a <= 0.01f) discard;
    
    return col;
    //return float4(ps_in.posW.xyz * 0.01f, 1.0f);
}