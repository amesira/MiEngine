//+++++++++++++++++++++++++++++++++++++++++++++++++++
// hologram_unlit_ps.hlsl
// 
// Date  ：2026/05/18
// Author：Miu Kitamura
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "../Common/camera.hlsl"
#include "../Common/material.hlsl"

Texture2D g_Texture : register(t6);
SamplerState g_SamplerState : register(s0);

Texture2D g_DitherTexture : register(t7); // ディザーテクスチャ

cbuffer HologramParams : register(b9) // CustomPropertyで設定した定数バッファ
{
    float4 g_HoloColor; // ホログラムの色
    float g_HoloIntensity; // ホログラムの強度
    float g_Time; // 時間（アニメーション用）
};

// 入力用構造体 [Sprite Pixel]
struct PS_INPUT
{
    float4 posH     : SV_Position;  // 変換済み頂点座標
    float4 posW     : POSITION1;    // ワールド座標
    float4 normal   : NORMAL0;      // 頂点法線
    float4 color    : COLOR0;       // 頂点カラー
    float2 texcoord : TEXCOORD0;    // テクスチャ座標
};

// main関数
float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float posY = ps_in.posW.y;
    float2 worldUV = float2(0.5, posY * 3.0) + float2(0, g_Time * 0.5); // UVをY軸に沿ってスクロールさせる
    worldUV.y = frac(worldUV.y); // Y軸繰り返す
    
    // テクスチャの色を取得・乗算
    float4 col = g_Texture.Sample(g_SamplerState, worldUV);
    
    float3 emission = g_HoloColor.rgb * g_HoloIntensity;
    col = float4(col.rgb + emission, col.a);
    
    // オブジェクトの端に行くほど明るくする
    float3 viewDir = normalize(g_EyePosition.xyz - ps_in.posW.xyz);
    float3 normal = normalize(ps_in.normal.xyz);
    float edge = 1.0f - saturate(abs(dot(viewDir, normal)));
    
    // エッジ部分を強調
    col.rgb *= edge * edge;
    col.rgb *= g_Material.baseColor.rgb; // マテリアルのベースカラーを乗算
    
    if (col.a <= 0.01f) {
        float2 screenUV = ps_in.posH.xy / ps_in.posH.w * 0.5 + 0.5; // スクリーンUVを計算
        float ditherValue = g_DitherTexture.Sample(g_SamplerState, screenUV).r; // ディザーテクスチャから値を取得
        if (col.a < ditherValue) {
            discard; // ディザリングでピクセルを破棄
        }
    }
    
    return col;
}