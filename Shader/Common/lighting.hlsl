//+++++++++++++++++++++++++++++++++++++++++++++++++++
// lighting.hlsl
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
    
    float   Intensity; // 光の強さ
    float3  padding2;
};

// PointLight構造体
#define POINT_LIGHT_MAX (4)
struct PointLight {
    int     Enable;     // ライトの有効・無効
    float3  padding;
    
    float4  Position;   // 光の位置
    float4  Diffuse;    // 光の色
    
    float   Intensity;  // 光の強さ
    float   Range;      // 光の届く距離
    float2  padding2;
};

// SpotLight構造体
#define SPOT_LIGHT_MAX (4)
struct SpotLight {
    bool    Enable;     // ライトの有効・無効
    float3  padding;
    
    float4  Position;   // 光の位置
    float4  Direction;  // 光の方向
    float4  Diffuse;    // 光の色
    
    float   Intensity;  // 光の強さ
    float   Range;      // 光の届く距離
    float   SpotAngle;  // スポットライトの角度（単位：ラジアン角）
    float   padding2;
};

// RimLight構造体
struct RimLight {
    bool    Enable;     // ライトの有効・無効
    float3  padding;
    
    float   Intensity;  // リムライトの強さ
    float   Threshold;  // リムライトの閾値（0~1の範囲で、値が小さいほど法線と視線が近い部分もリムライトが当たる）
    float2  padding2;
    
    float4  Color;      // リムライトの色
};

// HemiLight構造体
struct HemiLight {
    bool    Enable;     // ライトの有効・無効
    float3  padding;
    
    float   Intensity;  // ライトの強さ
    float3  padding2;
    
    float4  SkyColor;   // 空からの光の色
    float4  GroundColor;// 地面からの光の色
};

// ライト定数バッファ
cbuffer LightBuffer : register(b10)
{
    bool g_EnableLighting; // ライティング全体の有効・無効フラグ
    float3 padding;
    
    DirectionalLight g_DirectionalLights[DIRECTIONAL_LIGHT_MAX];
    PointLight g_PointLights[POINT_LIGHT_MAX];
    SpotLight g_SpotLights[SPOT_LIGHT_MAX];
    
    RimLight g_RimLight;
    HemiLight g_HemiLight;
}

// 拡散反射の計算関数（ディレクショナルライト用）
float3 CalcDiffuse_DirectionalLights(float3 normal)
{
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    
    // 法線を正規化
    float3 N = normalize(normal);
    
    for (int i = 0; i < DIRECTIONAL_LIGHT_MAX; i++)
    {
        // ライトを取得・有効かどうかを確認
        DirectionalLight light = g_DirectionalLights[i];
        if (light.Enable == 0) continue;
        
        // ライトの方向を正規化
        float3 L = normalize(-light.Direction.xyz);
        
        // ディフューズ成分を計算
        float NdotL = dot(N, L);
        if (NdotL <= 0.0f) continue;
        
        // ディフューズの色を加算
        diffuse += light.Diffuse.rgb * NdotL * light.Intensity;
    }
    
    return diffuse;
}

// 鏡面反射の計算関数（ディレクショナルライト用）
float3 CalcSpecular_DirectionalLights(float3 normal, float3 posW, float3 eyePos, float shininess)
{
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    
    // 法線を正規化
    float3 N = normalize(normal);
    
    for (int i = 0; i < DIRECTIONAL_LIGHT_MAX; i++)
    {
        // ライトを取得・有効かどうかを確認
        DirectionalLight light = g_DirectionalLights[i];
        if (light.Enable == 0) continue;
        
        // ライトの方向を正規化
        float3 L = normalize(-light.Direction.xyz);
        
        // 鏡面反射ベクトルを計算
        //float3 R = 2.0f * N * dot(N, L) - L;
        float3 R = reflect(-L, N);
        
        // 視点方向を計算
        float3 V = normalize(eyePos - posW);
        
        // スペキュラー成分を計算
        float RdotV = dot(R, V);
        if (RdotV <= 0.0f) continue;
        
        // スペキュラーの色を加算
        specular += light.Diffuse.rgb * pow(RdotV, shininess) * light.Intensity;
    }
    
    return specular;
}

// 拡散反射の計算関数（ポイントライト用）
float3 CalcDiffuse_PointLights(float3 normal, float3 posW)
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
        
        // ディフューズ成分を計算
        float NdotL = dot(N, L);
        if (NdotL <= 0.0f) continue;
        
        // 距離減衰を計算（距離が遠いほど減衰する。distance >= Rangeのときは0になる）
        float attenuation = saturate(1.0f - (distance / light.Range));
        attenuation *= attenuation; // 距離減衰を二次関数的に強める
        
        // ライトの色を加算
        lighting += light.Diffuse.rgb * NdotL * attenuation * light.Intensity;
    }
    
    return lighting;
}

// 鏡面反射の計算関数（ポイントライト用）
float3 CalcSpecular_PointLights(float3 normal, float3 posW, float3 eyePos, float shininess)
{
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    
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
        
        // 鏡面反射ベクトルを計算
        float3 R = reflect(-L, N);
        
        // 視点方向を計算
        float3 V = normalize(eyePos - posW);
        
        // スペキュラー成分を計算
        float RdotV = dot(R, V);
        if (RdotV <= 0.0f) continue;
        
        // 距離減衰を計算（距離が遠いほど減衰する。distance >= Rangeのときは0になる）
        float attenuation = saturate(1.0f - (distance / light.Range));
        attenuation *= attenuation; // 距離減衰を二次関数的に強める
        
        // スペキュラーの色を加算
        specular += light.Diffuse.rgb * pow(RdotV, shininess) * attenuation * light.Intensity;
    }
    
    return specular;
}

// 拡散反射の計算関数（スポットライト用）
float3 CalcDiffuse_SpotLights(float3 normal, float3 posW)
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
        lighting += light.Diffuse.rgb * NdotL * attenuation * light.Intensity;
    }
    
    return lighting;
}

// 鏡面反射の計算関数（スポットライト用）
float3 CalcSpecular_SpotLights(float3 normal, float3 posW, float3 eyePos, float shininess)
{
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    
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
        
        // 鏡面反射ベクトルを計算
        float3 R = reflect(-L, N);
        
        // 視点方向を計算
        float3 V = normalize(eyePos - posW);
        
        // スペキュラー成分を計算（0~1の範囲にclamp）
        float RdotV = saturate(dot(R, V));
        
        // 距離減衰を計算（距離が遠いほど減衰する。distance >= Rangeのときは0になる）
        float attenuation = saturate(1.0f - distance / light.Range);
        attenuation *= attenuation; // 距離減衰を二次関数的に強める
        
        // スペキュラーの色を加算
        specular += light.Diffuse.rgb * pow(RdotV, shininess) * attenuation * light.Intensity;
    }
    
    return specular;
}

// リムライトの計算関数
float3 CalcRimLight(float3 normal, float3 posW, float3 eyePos)
{
    if (g_RimLight.Enable == 0)
        return float3(0.0f, 0.0f, 0.0f);
    
    // Light方向と法線が逆行している場合 power1 = 1.0f になる
    float power1 = 0.0f;
    {
        float3 N = normalize(normal);
        float3 L = normalize(-g_DirectionalLights[0].Direction.xyz);
        float NdotL = dot(N, L);
        
        power1 = max(0.0f, NdotL);
        power1 = 1.0f - power1;
    }
    
    // View方向の逆と法線が逆行している場合 power2 = 1.0f になる
    float power2 = 0.0f;
    {
        float3 N = normalize(normal);
        float3 V = normalize(posW - eyePos);
        float VdotN = dot(-V, N);
        
        power2 = max(0.0f, VdotN);
        power2 = 1.0f - power2;
    }
    
    float rimPower = power1 * power2;
    rimPower = smoothstep(g_RimLight.Threshold, 1.0f, rimPower); // 閾値以下は0、以上は1になるように補間
    rimPower = pow(rimPower, 2.0f); // リムライトのエッジを強調するために二次関数的に強める
    
    return g_RimLight.Color.rgb * rimPower * g_RimLight.Intensity;
}

// 半球ライトの計算関数
float3 CalcHemiLight(float3 normal)
{
    if (g_HemiLight.Enable == 0)
        return float3(0.0f, 0.0f, 0.0f);
    
    float3 N = normalize(normal);
    float3 G = float3(0.0f, 1.0f, 0.0f); // 地面法線とする
    
    float NdotG = dot(N, G);
    float t = (NdotG + 1.0f) * 0.5f;
    
    return lerp(g_HemiLight.GroundColor.rgb, g_HemiLight.SkyColor.rgb, t) * g_HemiLight.Intensity;
}