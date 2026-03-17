//+++++++++++++++++++++++++++++++++++++++++++++++++++
// shader_light.hlsl
// ・ライトを定義するシェーダーファイル
// ・-b10-にライト定数バッファを割り当てる
// 
// Author：Miu Kitamura
// Date  ：2026/03/18
//+++++++++++++++++++++++++++++++++++++++++++++++++++

// DirectionalLight構造体
#define DIRECTIONAL_LIGHT_MAX (1)
struct DirectionalLight {
    bool    Enable;     // ライトの有効・無効
    float3  padding;
    
    float4  Direction;  // 光の方向
	float4  Diffuse;    // 光の色
    float4  Ambient;    // 環境光の色
};

// PointLight構造体
#define POINT_LIGHT_MAX (4)
struct PointLight {
    int     Enable;     // ライトの有効・無効
    float3  padding;
    
    float4  Position;   // 光の位置
    float4  Diffuse;    // 光の色
    
    float   Range;      // 光の届く距離
    float3  padding2;
};

// SpotLight構造体
#define SPOT_LIGHT_MAX (4)
struct SpotLight {
    bool    Enable;     // ライトの有効・無効
    float3  padding;
    
    float4  Position;   // 光の位置
    float4  Direction;  // 光の方向
    float4  Diffuse;    // 光の色
    
    float   Range;      // 光の届く距離
    float   SpotAngle;  // スポットライトの角度（単位：ラジアン角）
    float2  padding2;
};

// ライト定数バッファ
cbuffer LightBuffer : register(b10)
{
    bool                g_EnableLighting; // ライティング全体の有効・無効フラグ
    float3              padding;
    
    DirectionalLight g_DirectionalLights[DIRECTIONAL_LIGHT_MAX];
    PointLight g_PointLights[POINT_LIGHT_MAX];
    SpotLight g_SpotLights[SPOT_LIGHT_MAX];
}

// ライト計算関数（ディレクショナルライト用）
float3 CalcLight_DirectionalLights(float3 normal)
{
    float3 lighting = float3(0.0f, 0.0f, 0.0f);
    
    // 法線を正規化
    float3 N = normalize(normal);
    
    for (int i = 0; i < DIRECTIONAL_LIGHT_MAX; i++)
    {
        // ライトを取得・有効かどうかを確認
        DirectionalLight light = g_DirectionalLights[i];
        if (light.Enable == 0) continue;
        
        // ライトの方向を正規化
        float3 L = normalize(-light.Direction.xyz);
        
        // ディフューズ成分を計算（0~1の範囲にclamp）
        float NdotL = saturate(dot(N, L));
        
        // ライトの色を加算
        lighting += light.Diffuse.rgb * NdotL;
        lighting += light.Ambient.rgb; // 環境光はそのまま
    }
    
    return lighting;
}

// ライト計算関数（ポイントライト用）
float3 CalcLight_PointLights(float3 posW, float3 normal)
{
    float3 lighting = float3(0.0f, 0.0f, 0.0f);
    
    // 法線を正規化
    float3 N = normalize(normal);
    
    for (int i = 0; i < POINT_LIGHT_MAX; i++)
    {
        PointLight light = g_PointLights[i];
        if (light.Enable == 0)
            continue;
        if (light.Range <= 0.0f)
            continue;
        
        // ポイントライトの方向・距離を計算
        float3 L = light.Position.xyz - posW;
        float distance = length(L);
        if (distance <= 0.0001f || distance >= light.Range)
            continue; // 危険値・光源範囲外
        
        L = normalize(L); // 正規化
        
        // ディフューズ成分を計算（0~1の範囲にclamp）
        float NdotL = saturate(dot(N, L));
        
        // 距離減衰を計算（距離が遠いほど減衰する。distance >= Rangeのときは0になる）
        float attenuation = saturate(1.0f - distance / light.Range);
        //attenuation *= attenuation; // 距離減衰を二次関数的に強める
        
        // ライトの色を加算
        lighting += light.Diffuse.rgb * NdotL * attenuation;
    }
    
    return lighting;
}

// ライト計算関数（スポットライト用）
float3 CalcLight_SpotLights(float3 posW, float3 normal)
{
    float3 lighting = float3(0.0f, 0.0f, 0.0f);
    
    // 法線を正規化
    float3 N = normalize(normal);
    
    for (int i = 0; i < SPOT_LIGHT_MAX; i++)
    {
        SpotLight light = g_SpotLights[i];
        if (light.Enable == 0)
            continue;
        if (light.Range <= 0.0f)
            continue;
        if (light.SpotAngle <= 0.0f)
            continue;
        
        // スポットライトの方向・距離を計算
        float3 L = light.Position.xyz - posW;
        float distance = length(L);
        if (distance <= 0.0001f || distance >= light.Range)
            continue; // 危険値・光源範囲外
        
        L = normalize(L); // 正規化
        
        // そもそもSpotLightのAngle内にピクセルが位置するかどうか
        float3 D = normalize(light.Direction.xyz);
        float DdotL = dot(D, -L);
        if (DdotL < cos(light.SpotAngle))
            continue; // 光源範囲外
        
        // ディフューズ成分を計算（0~1の範囲にclamp）
        float NdotL = saturate(dot(N, L));
        
        // 距離減衰を計算（距離が遠いほど減衰する。distance >= Rangeのときは0になる）
        float attenuation = saturate(1.0f - distance / light.Range);
        attenuation *= attenuation; // 距離減衰を二次関数的に強める
        
        // ライトの色を加算
        lighting += light.Diffuse.rgb * NdotL * attenuation;
    }
    
    return lighting;
}