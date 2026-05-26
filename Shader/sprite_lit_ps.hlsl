//+++++++++++++++++++++++++++++++++++++++++++++++++++
// sprite_lit_ps.hlsl
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "./Common/camera.hlsl"
#include "./Common/lighting.hlsl"
#include "./Common/material.hlsl"
#include "./Common/shadowing.hlsl"

SamplerState g_SamplerState : register(s0);

struct PS_INPUT
{
    float4 posH     : SV_Position;
    float4 posW     : POSITION1;
    float4 normal   : NORMAL0;
    float4 color    : COLOR0;
    float2 texcoord : TEXCOORD0;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float2 uv = ps_in.texcoord * g_Material.uvTiling + g_Material.uvOffset;
    float4 col = g_Material.baseColor * ps_in.color * g_AlbedoTexture.Sample(g_SamplerState, uv);
    if (col.a <= 0.01f) discard;

    float3 normal = normalize(ps_in.normal.xyz);

    if (g_EnableLighting != 0)
    {
        float3 diffuseLight = g_DirectionalLights[0].Ambient;
        diffuseLight += CalcDiffuse_DirectionalLights(normal) * (1.0f - g_Material.metallic);
        diffuseLight += CalcDiffuse_PointLights(normal, ps_in.posW.xyz) * (1.0f - g_Material.metallic);
        diffuseLight += CalcDiffuse_SpotLights(normal, ps_in.posW.xyz) * (1.0f - g_Material.metallic);

        float3 specularLight = float3(0.0f, 0.0f, 0.0f);
        float shininess = lerp(256.0f, 2.0f, g_Material.roughness);
        specularLight += CalcSpecular_DirectionalLights(normal, ps_in.posW.xyz, g_EyePosition.xyz, shininess) * g_Material.metallic;
        specularLight += CalcSpecular_PointLights(normal, ps_in.posW.xyz, g_EyePosition.xyz, shininess) * g_Material.metallic;
        specularLight += CalcSpecular_SpotLights(normal, ps_in.posW.xyz, g_EyePosition.xyz, shininess) * g_Material.metallic;

        col.rgb *= diffuseLight;
        col.rgb += specularLight;
        //col.rgb += CalcRimLight(normal, ps_in.posW.xyz, g_EyePosition.xyz);
        col.rgb += CalcHemiLight(normal);

        float4 lightSpacePos = WorldToLightSpace(ps_in.posW);
        float2 shadowUV = CalcShadowUV(lightSpacePos);
        float depthInLightSpace = lightSpacePos.z / lightSpacePos.w;
        float depthInShadowMap = g_ShadowMap.Sample(g_SamplerState, shadowUV).r;

        float bias = 0.001f;
        if (depthInLightSpace > depthInShadowMap + bias)
        {
            col.rgb *= 0.5f;
        }
    }

    col.rgb += g_Material.emissiveColor * g_Material.emissiveIntensity;

    return col;
}
