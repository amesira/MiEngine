//+++++++++++++++++++++++++++++++++++++++++++++++++++
// skinned_lit_vs.hlsl [Skinned Lit頂点シェーダー]
// 
// Author：Miu Kitamura
// Date  ：2026/04/01
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "./Common/transform.hlsl"
#include "./Common/camera.hlsl"
#include "./Common/skinning.hlsl"

// 入力用頂点構造体 [Skinned Mesh Vertex]
struct VS_INPUT
{
    float4 posL     : POSITION0;    // 頂点座標
    float4 normal   : NORMAL0;      // 頂点法線
    float4 color    : COLOR0;       // 頂点カラー（使用しないが、構造を合わせるために定義）
    float2 texcoord : TEXCOORD0;    // テクスチャ座標（U,V）
    
    // スキニング用のインデックスとウェイト
    uint4  boneIndex  : BLENDINDICES0; // ボーンインデックス
    float4 boneWeight : BLENDWEIGHT0;  // ボーンウェイト
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
    
    // スキニング処理
    float4x4 skinMatrix = CalcSkinningMatrix(vs_in.boneIndex, vs_in.boneWeight);
    float4 skinPosL = mul(vs_in.posL, skinMatrix);
    
    // 頂点を行列変換
    vs_out.posW = mul(skinPosL, g_WorldMatrix);
    vs_out.posH = mul(mul(vs_out.posW, g_ViewMatrix), g_ProjectionMatrix);
    
    // スキニング処理（法線）
    float3x3 skinNormalMatrix = (float3x3)skinMatrix;
    float3 skinnedNormal = mul(vs_in.normal.xyz, skinNormalMatrix);
    
    // 法線をワールド空間に変換して正規化
    float3x3 normalMatrix = (float3x3)g_WorldMatrix;
    vs_out.normal.xyz = normalize(mul(skinnedNormal, normalMatrix));
    
    // テクスチャ座標
    vs_out.texcoord = vs_in.texcoord;
    
    return vs_out;
}