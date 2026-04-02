//+++++++++++++++++++++++++++++++++++++++++++++++++++
// material.hlsl
// ・マテリアルを定義するシェーダーファイル
// ・-b9-にマテリアル定数バッファを割り当てる
// ・-t0- ~ -t3-にテクスチャを割り当てる
// ・-s1-にサンプラーステートを割り当てる
// 
// Author：Miu Kitamura
// Date  ：2026/03/30
//+++++++++++++++++++++++++++++++++++++++++++++++++++

// マテリアル構造体
struct Material {
    float4  baseColor;  // ベースカラー（R,G,B,A）
    float   metallic;   // 金属度（0.0 = 非金属, 1.0 = 金属）
    float   roughness;  // 粗さ（0.0 = 滑らか, 1.0 = 粗い）
    float2 padding;
    
    float3  emissiveColor; // 自己発光色（R,G,B）
    float padding2;
    
    float2  uvTiling;   // UVのループ数
    float2  uvOffset;   // UVのオフセット
};

// マテリアル定数バッファ
cbuffer MaterialBuffer : register(b9)
{
    Material g_Material;
};

// アルベドテクスチャ
Texture2D g_AlbedoTexture : register(t0);
SamplerState g_SamplerState : register(s0);
// 法線マップ
Texture2D g_NormalTexture : register(t1);
// 金属度・粗さテクスチャ
Texture2D g_MetallicRoughnessTexture : register(t2);
// エミッシブテクスチャ
Texture2D g_EmissiveTexture : register(t3);
