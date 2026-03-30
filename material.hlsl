//+++++++++++++++++++++++++++++++++++++++++++++++++++
// material.hlsl
// ・マテリアルを定義するシェーダーファイル
// ・-b11-にマテリアル定数バッファを割り当てる
// ・-t1- ~ -t4-にテクスチャを割り当てる
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
    float padding[2];
    
    float3  emissiveColor; // 自己発光色（R,G,B）
    float padding2;
    
    float2  uvTiling;   // UVのループ数
    float2  uvOffset;   // UVのオフセット
};

// マテリアル定数バッファ
cbuffer MaterialBuffer : register(b11)
{
    Material g_Material;
};

// アルベドテクスチャ
Texture2D g_AlbedoTexture : register(t1);
SamplerState g_SamplerState : register(s1);
// 法線マップ
Texture2D g_NormalTexture : register(t2);
// 金属度・粗さテクスチャ
Texture2D g_MetallicRoughnessTexture : register(t3);
// エミッシブテクスチャ
Texture2D g_EmissiveTexture : register(t4);

// 粗さからスペキュラの鋭さを計算する関数
float GetShininess(){
    return lerp(64.0f, 2.0f, g_Material.roughness);
}