//+++++++++++++++++++++++++++++++++++++++++++++++++++
// particle_vs.hlsl [Particle頂点シェーダー]
// 
// Author：Miu Kitamura
// Date  ：2026/06/01
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "./Common/camera.hlsl"

// 入力用頂点構造体 [Particle Vertex]
struct VS_INPUT
{
    float3 posL      : POSITION0;
    float2 texcoord  : TEXCOORD0;

    float4 world0    : WORLD0;
    float4 world1    : WORLD1;
    float4 world2    : WORLD2;
    float4 world3    : WORLD3;
    float4 color     : COLOR0;
    float4 uvRect    : TEXCOORD1;
};

// 出力用頂点構造体 [Particle Vertex]
struct VS_OUTPUT
{
    float4 posH      : SV_Position;
    float4 color     : COLOR0;
    float2 texcoord  : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;

    // ワールド行列を構築
    float4x4 world = float4x4(
        vs_in.world0,
        vs_in.world1,
        vs_in.world2,
        vs_in.world3
    );

    // 頂点を行列変換
    float4 posW = mul(float4(vs_in.posL, 1.0f), world);
    vs_out.posH = mul(mul(posW, g_ViewMatrix), g_ProjectionMatrix);
    vs_out.color = vs_in.color;
    vs_out.texcoord = vs_in.uvRect.xy + vs_in.texcoord * vs_in.uvRect.zw;

    return vs_out;
}
