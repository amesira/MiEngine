//+++++++++++++++++++++++++++++++++++++++++++++++++++
// lit_vs.hlsl [Lit頂点シェーダー]
// 
// Author：Miu Kitamura
// Date  ：2026/04/01
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "transform.hlsl"
#include "camera.hlsl"

// 入力用頂点構造体 [Static Mesh Vertex]
struct VS_INPUT
{
    float4 posL     : POSITION0;    // 頂点座標
    float4 normal   : NORMAL0;      // 頂点法線
    float2 texcoord : TEXCOORD0;    // テクスチャ座標（U,V）
};

// 出力用頂点構造体 [Mesh Vertex]
struct VS_OUTPUT
{
    float4 posH     : SV_Position;  // 変換済み頂点座標
    float4 posW     : POSITION1;    // ワールド座標
    float4 normal   : NORMAL0;      // 頂点法線
    float2 texcoord : TEXCOORD0; // テクスチャ座標
};

// main関数
VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
    
    // 頂点を行列変換
    vs_out.posW = mul(vs_in.posL, g_WorldMatrix);
    vs_out.posH = mul(mul(vs_out.posW, g_ViewMatrix), g_ProjectionMatrix);
    
    // 法線変換
    // （g_NormalMatrixを使う予定）
    float3x3 normalMatrix = (float3x3)g_WorldMatrix;
    vs_out.normal.xyz = normalize(mul(vs_in.normal.xyz, normalMatrix));
    
    // テクスチャ座標
    vs_out.texcoord = vs_in.texcoord;
    
    return vs_out;
}