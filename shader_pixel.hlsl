/*==============================================================================

   2D描画用ピクセルシェーダー [shader_pixel.hlsl]
--------------------------------------------------------------------------------

==============================================================================*/
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

cbuffer Option : register(b0)
{
    float4 colorRate;
    float4 alphaColor;
    float  grayRate; // 0.0 = 通常, 1.0 = 完全グレー
};

struct PS_INPUT // VS_OUTPUTと同じ内容
{
    float4 posH : SV_Position;   // ピクセルの座標
    float4 color : COLOR0;       // ピクセルの色
    float2 texcoord : TEXCOORD0; // テクスチャ座標
};

// pixelシェーダーは塗りつぶし（ピクセル）の色を決めるためだけのシェーダーなので、
// 返り値は必ずfloat4になる。
// ※返り値の色は:SV_TARGETにする決まり。
float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 col;
    
    // テクスチャの色を取得
    col = g_Texture.Sample(g_SamplerState, ps_in.texcoord);
    col *= ps_in.color;
    
    // アルファカラーと近い色を透明にする
    if (alphaColor.a < 0.01f)
    {
        float alphaDiff = length(col.rgb - alphaColor.rgb);
        if (alphaDiff < 0.1f)
        {
            col.a = 0.0f;
        }
    }
    
    col *= colorRate;
    
    // グレースケール化
    float gray = dot(col.rgb, float3(0.299, 0.587, 0.114));
    float3 grayColor = float3(gray, gray, gray);
    col.rgb = lerp(col.rgb, grayColor, grayRate);
    
    // 透明だった場合は処理を抜ける
    // -zバッファに書き込まないようにするため-
    if (col.a <= 0.01f)
    {
        discard;
        //return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    return col;
}