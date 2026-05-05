//+++++++++++++++++++++++++++++++++++++++++++++++++++
// liquid_surface_lit_ps.hlsl
// 
// Author：Miu Kitamura
// Date  ：2026/05/04
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "./Common/transform.hlsl"
#include "./Common/camera.hlsl"
#include "./Common/lighting.hlsl"
#include "./Common/material.hlsl"
#include "./Common/shadowing.hlsl"

SamplerState g_SamplerState : register(s0);

// 高さマップテクスチャ
Texture2D g_HeightMapNormalTexture : register(t5);

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
    
    // 高さマップから法線を取得
    float3 localNormal = g_HeightMapNormalTexture.Sample(g_SamplerState, ps_in.texcoord).xyz * 2.0f - 1.0f;
    float3 worldNormal = normalize(mul(localNormal, (float3x3)g_WorldMatrix));
    ps_in.normal.xyz = worldNormal;
    
    // テクスチャの色を取得・乗算
    col = g_Material.baseColor * g_AlbedoTexture.Sample(g_SamplerState, ps_in.texcoord);
    if (col.a <= 0.01f) discard;
    
    // 法線マップを使用して法線を変換
    
    // ライトの影響を加算
    if (g_EnableLighting != 0)
    {
        // ディフューズ光の計算
        float3 diffuseLight = g_DirectionalLights[0].Ambient;
        diffuseLight += CalcDiffuse_DirectionalLights(ps_in.normal.xyz) * (1.0f - g_Material.metallic);
        diffuseLight += CalcDiffuse_PointLights(ps_in.normal.xyz, ps_in.posW.xyz) * (1.0f - g_Material.metallic);
        diffuseLight += CalcDiffuse_SpotLights(ps_in.normal.xyz, ps_in.posW.xyz) * (1.0f - g_Material.metallic);
        
        // スペキュラ光の計算
        float3 specularLight = float3(0, 0, 0);
        float shininess = lerp(256.0f, 2.0f, g_Material.roughness);
        specularLight += CalcSpecular_DirectionalLights(ps_in.normal.xyz, ps_in.posW.xyz, g_EyePosition.xyz, shininess) * g_Material.metallic;
        specularLight += CalcSpecular_PointLights(ps_in.normal.xyz, ps_in.posW.xyz, g_EyePosition.xyz, shininess) * g_Material.metallic;
        specularLight += CalcSpecular_SpotLights(ps_in.normal.xyz, ps_in.posW.xyz, g_EyePosition.xyz, shininess) * g_Material.metallic;
        
        col.rgb *= diffuseLight; // ディフューズ光を乗算
        col.rgb += specularLight; // スペキュラ光を加算
        
        col.rgb += CalcRimLight(ps_in.normal.xyz, ps_in.posW.xyz, g_EyePosition.xyz); // リムライトを加算
        col.rgb += CalcHemiLight(ps_in.normal.xyz); // 半球ライトを加算
    }
    
    // エミッシブカラーを加算
    col.rgb += g_Material.emissiveColor;
    
    // シャドウマッピングの影響を減算
    float4 lightSpacePos = WorldToLightSpace(ps_in.posW);
    float2 shadowUV = CalcShadowUV(lightSpacePos);
    
    float depthInLightSpace = lightSpacePos.z / lightSpacePos.w;
    float depthInShadowMap = g_ShadowMap.Sample(g_SamplerState, shadowUV).r;
    
    float bias = 0.001f;
    if (depthInLightSpace > depthInShadowMap + bias){
        col.rgb *= 0.5f;
    }
    
    return col;
}