//+++++++++++++++++++++++++++++++++++++++++++++++++++
// shader_vertex.hlsl
// 
// Author：Miu Kitamura
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// 行列定数バッファ
cbuffer Buffer0 : register(b0)
{
    float4x4 mtx;
}

// 
cbuffer Buffer1:register(b1)
{
    float4x4 World;
}

// 入力用頂点構造体
struct VS_INPUT
{
    // slot0: 頂点座標
    float4 posL     : POSITION0;    // 頂点座標
    float4 normal   : NORMAL0;      // 頂点法線
    float4 color    : COLOR0;       // 頂点カラー（R,G,B,A）
    float2 texcoord : TEXCOORD0;    // テクスチャ座標（U,V）
    
    // slot1: インスタンスバッファ
    float4 ins_world0 : INS_WORLD0; // インスタンス用ワールド行列0
    float4 ins_world1 : INS_WORLD1; // インスタンス用ワールド行列1
    float4 ins_world2 : INS_WORLD2; // インスタンス用ワールド行列2
    float4 ins_world3 : INS_WORLD3; // インスタンス用ワールド行列3
    float4 ins_color : INS_COLOR0; // インスタンスカラー（R,G,B,A）
    float4 ins_uvrect : INS_UVRECT0; // インスタンスUV矩形（U,V,幅,高）
};

// 出力用頂点構造体
struct VS_OUTPUT
{
    float4 posH     : SV_Position;  // 変換済み頂点座標
    float4 posW     : POSITION1;    // ワールド座標
    float4 normal   : NORMAL0;      // 頂点法線
    float4 color    : COLOR0;       // 頂点カラー
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
    
    // 頂点を行列変換
    float4x4 worldMatrix = float4x4(
        vs_in.ins_world0,
        vs_in.ins_world1,
        vs_in.ins_world2,
        vs_in.ins_world3
    );
    vs_out.posW = mul(vs_in.posL, worldMatrix);
    vs_out.posH = mul(vs_out.posW, mtx);
    
    // 法線を行列変換
    vs_out.normal = mul(vs_in.normal, worldMatrix);
    
    // 頂点カラー
    vs_out.color = vs_in.color * vs_in.ins_color;
    
    // テクスチャ座標
    vs_out.texcoord = vs_in.texcoord * vs_in.ins_uvrect.zw + vs_in.ins_uvrect.xy;
    
    return vs_out;
}