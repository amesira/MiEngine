//+++++++++++++++++++++++++++++++++++++++++++++++++++
// shader_vertex.hlsl
// 
// Author：Miu Kitamura
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "transform.hlsl"
#include "camera.hlsl"

// 入力用頂点構造体
struct VS_INPUT
{
    float4 posL     : POSITION0;    // 頂点座標
    float4 normal   : NORMAL0;      // 頂点法線
    float4 color    : COLOR0;       // 頂点カラー（R,G,B,A）
    float2 texcoord : TEXCOORD0;    // テクスチャ座標（U,V）
};

// 出力用頂点構造体
struct VS_OUTPUT
{
    float4 posH     : SV_Position;  // 変換済み頂点座標
    float4 posW     : POSITION1;    // ワールド座標
    float4 normal   : NORMAL0;      // 頂点法線
    float4 color    : COLOR0;       // 頂点カラー
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
    vs_in.posL.w = 1.0f; // 同次座標のw成分を1に設定
    
    // 頂点を行列変換
    vs_out.posW = mul(vs_in.posL, g_WorldMatrix);
    
    vs_out.posH = mul(mul(vs_out.posW, g_ViewMatrix), g_ProjectionMatrix);
    
    // 法線変換
    float3x3 normalMatrix = (float3x3)g_WorldMatrix;
    vs_out.normal.xyz = normalize(mul(vs_in.normal, normalMatrix));
    
    // 頂点カラー
    vs_out.color = vs_in.color;
    
    // テクスチャ座標
    vs_out.texcoord = vs_in.texcoord;
    
    return vs_out;
}