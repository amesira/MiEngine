//+++++++++++++++++++++++++++++++++++++++++++++++++++
// full_screen_vs.hlsl
// 
// Author：Miu Kitamura
// Date  ：2026/05/18
//+++++++++++++++++++++++++++++++++++++++++++++++++++

struct VS_OUTPUT {
    float4 posH : SV_Position;
    float2 texcoord   : TEXCOORD;
};

// 頂点IDを使用してフルスクリーントライアングルの頂点位置とUV座標を生成
VS_OUTPUT main(uint id : SV_VertexID)
{
    VS_OUTPUT output;

    float2 pos[3] = {
        float2(-1.0f, -1.0f),
        float2(-1.0f,  3.0f),
        float2( 3.0f, -1.0f)
    };

    float2 texcoord[3] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, -1.0f),
        float2(2.0f, 1.0f)
    };
    //float2 texcoord[3] = {
    //    float2(0.0f, 0.0f),
    //    float2(0.0f, 2.0f),
    //    float2(2.0f, 0.0f)
    //};

    output.posH = float4(pos[id], 0.0f, 1.0f);
    output.texcoord = texcoord[id];

    return output;
}