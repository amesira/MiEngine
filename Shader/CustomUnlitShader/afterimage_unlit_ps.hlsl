//+++++++++++++++++++++++++++++++++++++++++++++++++++
// afterimage_unlit_ps.hlsl
//+++++++++++++++++++++++++++++++++++++++++++++++++++
#include "../Common/material.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_DitherTexture : register(t6);
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
    float4 col = g_Texture.Sample(g_SamplerState, ps_in.texcoord);
    if (col.a < 0.01f) {
        discard; // 透明なピクセルは描画しない
    }
    
    col *= ps_in.color;
    col *= g_Material.baseColor;

    if (col.a <= 0.01f) {
        float2 screenUV = ps_in.posH.xy / ps_in.posH.w * 0.5 + 0.5; // スクリーンUVを計算
        float ditherValue = g_DitherTexture.Sample(g_SamplerState, screenUV).r; // ディザーテクスチャから値を取得
        if (col.a < ditherValue) {
            discard; // ディザリングでピクセルを破棄
        }
    }

    return col;
}
