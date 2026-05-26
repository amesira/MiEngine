//+++++++++++++++++++++++++++++++++++++++++++++++++++
// sprite_unlit_vs.hlsl
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "./Common/transform.hlsl"
#include "./Common/camera.hlsl"

// Input vertex structure [Sprite Vertex]
struct VS_INPUT
{
    float4 posL     : POSITION0;
    float4 normal   : NORMAL0;
    float4 color    : COLOR0;
    float2 texcoord : TEXCOORD0;
};

// Output vertex structure [Sprite Unlit Vertex]
struct VS_OUTPUT
{
    float4 posH     : SV_Position;
    float4 posW     : POSITION1;
    float4 normal   : NORMAL0;
    float4 color    : COLOR0;
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;

    vs_out.posW = mul(vs_in.posL, g_WorldMatrix);
    vs_out.posH = mul(mul(vs_out.posW, g_ViewMatrix), g_ProjectionMatrix);

    float3x3 normalMatrix = (float3x3)g_WorldMatrix;
    vs_out.normal.xyz = normalize(mul(vs_in.normal.xyz, normalMatrix));
    vs_out.normal.w = 0.0f;

    vs_out.color = vs_in.color;
    vs_out.texcoord = vs_in.texcoord;

    return vs_out;
}
