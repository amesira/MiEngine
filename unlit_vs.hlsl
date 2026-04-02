//+++++++++++++++++++++++++++++++++++++++++++++++++++
// unlit_vs.hlsl
// 
// Author：Miu Kitamura
// Date  ：2026/04/01
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "transform.hlsl"
#include "camera.hlsl"

// 入力用頂点構造体 [Sprite Vertex]
struct VS_INPUT
{
    float4 posL     : POSITION0;    // 頂点座標
    float4 color    : COLOR0;       // 頂点カラー
    float2 texcoord : TEXCOORD0;    // テクスチャ座標（U,V）
};

// 出力用頂点構造体 [Sprite Vertex]
struct VS_OUTPUT
{
    float4 posH     : SV_Position; // 変換済み頂点座標
    float4 color    : COLOR0;       // 頂点カラー
    float2 texcoord : TEXCOORD0;    // テクスチャ座標
};

// main関数
VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
    
    vs_out.posH = mul(vs_in.posL, g_WorldMatrix);
    vs_out.posH = mul(mul(vs_out.posH, g_ViewMatrix), g_ProjectionMatrix);
    
    // 色
    vs_out.color = vs_in.color;
    
    // テクスチャ座標
    vs_out.texcoord = vs_in.texcoord;
    
    return vs_out;
}