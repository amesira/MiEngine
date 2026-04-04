//+++++++++++++++++++++++++++++++++++++++++++++++++++
// skinning.hlsl
// ・スキニング処理を行うシェーダーコード
// 
// Author：Miu Kitamura
// Date  ：2026/04/04
//+++++++++++++++++++++++++++++++++++++++++++++++++++

// スキニング定数バッファ
cbuffer SkinningBuffer : register(b12)
{
    float4x4 g_BoneMatrix[256]; // 各ボーンの変換行列
};

// スキニング変換行列の計算関数
float4x4 CalcSkinningMatrix(int4 boneIndex, float4 boneWeight)
{
    float4x4 skinMatrix;
    skinMatrix = mul(g_BoneMatrix[boneIndex.x], boneWeight.x);
    skinMatrix += mul(g_BoneMatrix[boneIndex.y], boneWeight.y);
    skinMatrix += mul(g_BoneMatrix[boneIndex.z], boneWeight.z);
    skinMatrix += mul(g_BoneMatrix[boneIndex.w], boneWeight.w);
    
    return skinMatrix;
}
