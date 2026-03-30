//+++++++++++++++++++++++++++++++++++++++++++++++++++
// transform.hlsl
// ・物体の座標行列を定義するシェーダーファイル
// ・-b0-にワールド行列定数バッファを割り当てる
//
// Author：Miu Kitamura
// Date  ：2026/03/30
//+++++++++++++++++++++++++++++++++++++++++++++++++++

// Transform定数バッファ
cbuffer TransformBuffer : register(b0)
{
    float4x4 g_WorldMatrix; // ワールド行列
    float4x4 g_NormalMatrix;// 法線変換行列
};