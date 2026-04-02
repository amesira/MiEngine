//+++++++++++++++++++++++++++++++++++++++++++++++++++
// ttf_unlit_ps.hlsl
// 
// Author：Miu Kitamura
//+++++++++++++++++++++++++++++++++++++++++++++++++++
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// 入力用構造体 [Sprite Pixel]
struct PS_INPUT
{
    float4 posH     : SV_Position;  // 変換済み頂点座標
    float4 color    : COLOR0;       // 頂点カラー
    float2 texcoord : TEXCOORD0;    // テクスチャ座標
};

// main関数
float4 main(PS_INPUT ps_in) : SV_TARGET
{
    // 頂点カラーを初期値とする
    float4 col = ps_in.color;
    
    // テクスチャの色を取得・乗算
    // ・文字のテクスチャはグレースケールなので、R成分をアルファ値として使用
    col *= float4(1.0, 1.0, 1.0, g_Texture.Sample(g_SamplerState, ps_in.texcoord).r);
    
    if (col.a <= 0.01f) discard;
    return col;
}