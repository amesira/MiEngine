//+++++++++++++++++++++++++++++++++++++++++++++++++++
// camera.hlsl
// ・カメラのビュー行列とプロジェクション行列、視点座標を定義する
//   シェーダーファイル
// ・-b1-にカメラ定数バッファを割り当てる
// 
// Author：Miu Kitamura
// Date  ：2026/03/30
//+++++++++++++++++++++++++++++++++++++++++++++++++++

// カメラ定数バッファ
cbuffer CameraBuffer : register(b1)
{
    float4x4 g_ViewMatrix;           // ビュー行列
    float4x4 g_ProjectionMatrix;     // プロジェクション行列
    float4   g_EyePosition;          // 視点の位置（ワールド座標）
    
    float4x4 g_InvViewMatrix;       // ビュー行列の逆行列
    float4x4 g_InvProjectionMatrix; // プロジェクション行列の逆行列
};