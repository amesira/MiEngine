//+++++++++++++++++++++++++++++++++++++++++++++++++++
// shadowing.hlsl
// ・シャドウマッピング用のシェーダーファイル
// ・-b11-にシャドウ用ライト行列定数バッファを割り当てる
// ・-t10-にシャドウマップテクスチャを割り当てる
// 
// Author：Miu Kitamura
// Date  ：2026/04/03
//+++++++++++++++++++++++++++++++++++++++++++++++++++

// シャドウ用ライト行列
cbuffer ShadowMatrixBuffer : register(b11)
{
    float4x4 g_LightViewProjMatrix; // ライトのビュー射影行列
};

// シャドウマップテクスチャ
Texture2D g_ShadowMap : register(t10);

// ワールド座標をライト空間座標に変換
float4 WorldToLightSpace(float4 posW)
{
    return mul(posW, g_LightViewProjMatrix);
}

// ライト空間座標からシャドウマップのUV座標を計算
float2 CalcShadowUV(float4 lightWorldPos)
{
    // ライト空間座標をNDC空間に変換
    float2 shadowUV = lightWorldPos.xy / lightWorldPos.w * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y;
    shadowUV = clamp(shadowUV, 0.0f, 1.0f);
    
    return shadowUV;
}