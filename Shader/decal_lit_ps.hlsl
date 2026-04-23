//+++++++++++++++++++++++++++++++++++++++++++++++++++
// decal_lit_ps.hlsl [DecalLitピクセルシェーダー]
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "./Common/transform.hlsl"
#include "./Common/camera.hlsl"

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
    
    // スクリーン座標をテクスチャ座標に変換
    float2 screenUV = ps_in.posH.xy / float2(1920.0f, 1080.0f); // 画面解像度で割る
    screenUV = clamp(screenUV, 0.0f, 1.0f);
    
    // 深度テクスチャから深度値を取得
    float depth = g_DepthTexture.Sample(g_SamplerState, screenUV).r;
    
    // スクリーン座標と深度値を使ってワールド座標を復元
    float4 worldPosFromDepth;
    {
        // クリップ空間座標を計算
        float2 ndc;
        ndc.x = screenUV.x * 2.0f - 1.0f;
        ndc.y = (1.0f - screenUV.y) * 2.0f - 1.0f;
        float4 clipPos = float4(ndc, depth, 1.0f);
        
        // View空間へ
        float4 viewPos = mul(clipPos, g_InvProjectionMatrix);
        viewPos /= viewPos.w;
        
        // ワールド空間へ
        worldPosFromDepth = mul(viewPos, g_InvViewMatrix);
    }
    
    // ワールド座標をデカールローカル空間に変換
    float4 localPos = mul(worldPosFromDepth, g_InvWorldMatrix);
    
    // ローカル空間での位置が-1～1の範囲外なら描画しない
    if (abs(localPos.x) >= 1.0f || abs(localPos.y) >= 1.0f || abs(localPos.z) >= 1.0f)
        discard;
    
    // テクスチャの色を取得・乗算
    float2 decalUV = localPos.xy * 0.5f + 0.5f; // ローカル座標をUVに変換
    decalUV.y = 1.0f - decalUV.y; // Y軸を反転
    col = g_Texture.Sample(g_SamplerState, decalUV);
    if (col.a <= 0.01f)
        discard;
    
    return col;
}