//+++++++++++++++++++++++++++++++++++++++++++++++++++
// decal_lit_ps.hlsl [DecalLitピクセルシェーダー]
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "transform.hlsl"
#include "camera.hlsl"

SamplerState g_SamplerState : register(s0);

Texture2D g_Texture : register(t0);
Texture2D g_DepthTexture : register(t5);

// ピクセルシェーダーの入力構造体
struct PS_INPUT // VS_OUTPUTと同じ内容
{
    float4 posH     : SV_Position;  // ピクセルの座標
    float4 posW     : POSITION1;    // ワールド座標
    float4 normal   : NORMAL0;      // ピクセルの法線
    float2 texcoord : TEXCOORD0;    // テクスチャ座標
};

// ピクセルシェーダーのメイン関数
float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 col = float4(0, 0, 0, 1);
    
    // スクリーン座標をNDC空間に変換
    float2 screenUV = ps_in.posH.xy / float2(1920.0f, 1080.0f); // 画面解像度で割る
    screenUV = clamp(screenUV, 0.0f, 1.0f);
    screenUV.x = screenUV.x * 2.0f - 1.0f;          // [0,1] -> [-1,1]
    screenUV.y = 1.0f - screenUV.y * 2.0f + 1.0f;   // [0,1] -> [1,-1]
    
    // 深度テクスチャから深度値を取得
    float depth = g_DepthTexture.Sample(g_SamplerState, screenUV).r;
    
    // スクリーン座標と深度値を使ってワールド座標を復元
    float4 worldPosFromDepth;
    {
        // クリップ空間座標を計算
        float4 clipPos = float4(screenUV, depth, 1.0f);
        
        // View空間へ
        float4 viewPos = mul(clipPos, g_InvProjectionMatrix);
        viewPos /= viewPos.w;
        
        // ワールド空間へ
        worldPosFromDepth = mul(viewPos, g_InvViewMatrix);
    }
    
    // ワールド座標をデカールローカル空間に変換
    float4 localPos = mul(worldPosFromDepth, g_InvWorldMatrix);
    
    depth -= 0.5f;
    
    //return col;
    return float4(depth, depth, depth, 1.0f); // デバッグ用：深度値をグレースケールで表示)
}