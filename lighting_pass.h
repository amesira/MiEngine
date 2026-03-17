//---------------------------------------------------
// lighting_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/05
//---------------------------------------------------
#ifndef LIGHTING_PASS_H
#define LIGHTING_PASS_H
#include "pass.h"

#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#pragma region ライトの構造体定義
// DirectionalLight構造体
struct alignas(16) GPU_DirectionalLight {
    int enable;
    float padding[3];

    XMFLOAT4 direction;
    XMFLOAT4 diffuse;
    XMFLOAT4 ambient;
};

// PointLight構造体
struct alignas(16) GPU_PointLight {
    int enable;
    float padding[3];

    XMFLOAT4 position;
    XMFLOAT4 diffuse;

    float range;
    float padding2[3];
};

// SpotLight構造体
struct alignas(16) GPU_SpotLight {
    int enable;
    float padding[3];

    XMFLOAT4 position;
    XMFLOAT4 direction;
    XMFLOAT4 diffuse;

    float range;
    float spotAngle;
    float padding2[2];
};
#pragma endregion

class LightingPass : public Pass {
private:
    static const int DIRECTIONAL_LIGHT_MAX = 1;
    static const int POINT_LIGHT_MAX = 4;
    static const int SPOT_LIGHT_MAX = 4;

    struct alignas(16) LightBufferData {
        int enableLighting; // ライティング全体の有効・無効
        float padding[3];

        GPU_DirectionalLight directionalLights[DIRECTIONAL_LIGHT_MAX];
        GPU_PointLight  pointLights[POINT_LIGHT_MAX];
        GPU_SpotLight   spotLights[SPOT_LIGHT_MAX];
    } m_lightBufferData;
    ComPtr<ID3D11Buffer> m_lightBuffer;

    int m_lightCount;

public:
    void Initialize() override;
    void Finalize() override;
    void Process(IScene* pScene) override;

    void BindLightBuffer();
    void SetLightEnable(bool enable);

};

#endif // LIGHT_PROCESSOR_H