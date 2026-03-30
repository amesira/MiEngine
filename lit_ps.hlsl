//+++++++++++++++++++++++++++++++++++++++++++++++++++
// lit_ps.hlsl
// 
// Author：Miu Kitamura
// Date  ：2026/03/30
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "camera.hlsl"
#include "lighting.hlsl"
#include "material.hlsl"

// ピクセルシェーダーの入力構造体
struct PS_INPUT // VS_OUTPUTと同じ内容
{
    float4 posH     : SV_Position;  // ピクセルの座標
    float4 posW     : POSITION1;    // ワールド座標
    float4 normal   : NORMAL0;      // ピクセルの法線
    float4 color    : COLOR0;       // ピクセルの色
    float2 texcoord : TEXCOORD0;    // テクスチャ座標
};

// ピクセルシェーダーのメイン関数
float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 col = float4(0, 0, 0, 1); // 初期値は黒（不透明）
    
   // // テクスチャの色を取得・乗算
   //// col = g_AlbedoTexture.Sample(g_SamplerState, ps_in.texcoord);
   // if (col.a <= 0.01f) discard;
    
   // // 法線マップを使用して法線を変換
   // // https://www.opengl-tutorial.org/jp/intermediate-tutorials/tutorial-13-normal-mapping/
   // //float4 normal = g_NormalTexture.Sample(g_SamplerState, ps_in.texcoord) * 2.0f - 1.0f;
    
   // // 視点位置に向かってのベクトルを計算
   // float3 toEye = normalize(g_EyePosition.rgb - ps_in.posW.xyz);
    
   // // ライトの影響を加算
   // if (g_EnableLighting != 0)
   // {
   //     col.rgb += CalcDiffuse_DirectionalLights(ps_in.normal.xyz);
   //     col.rgb += CalcLight_PointLights(ps_in.posW.xyz, ps_in.normal.xyz);
   //     col.rgb += CalcLight_SpotLights(ps_in.posW.xyz, ps_in.normal.xyz);
   // }
    
   // // マテリアルのベースカラーとライトの環境光を加算
   // col.rgb += g_Material.baseColor.rgb * g_DirectionalLights[0].Ambient.rgb;
    
    return col;
}