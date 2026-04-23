//---------------------------------------------------
// lighting_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/05
//---------------------------------------------------
#ifndef LIGHTING_PASS_H
#define LIGHTING_PASS_H
#include "Engine/Core/pass.h"

#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#pragma region ライトの構造体定義
// DirectionalLight構造体
struct alignas(16) GPU_DirectionalLight {
    int     enable;
    float   padding[3];

    XMFLOAT4 direction;
    XMFLOAT4 diffuse;
    XMFLOAT4 ambient;

    float   intensity;
    float   padding2[3];
};

// PointLight構造体
struct alignas(16) GPU_PointLight {
    int     enable;
    float   padding[3];

    XMFLOAT4 position;
    XMFLOAT4 diffuse;

    float   intensity;
    float   range;
    float   padding2[2];
};

// SpotLight構造体
struct alignas(16) GPU_SpotLight {
    int enable;
    float padding[3];

    XMFLOAT4 position;
    XMFLOAT4 direction;
    XMFLOAT4 diffuse;

    float   intensity;
    float   range;
    float   spotAngle;
    float   padding2[1];
};
#pragma endregion

class LightingPass : public Pass {
private:
    static const int DIRECTIONAL_LIGHT_MAX = 1;
    static const int POINT_LIGHT_MAX = 4;
    static const int SPOT_LIGHT_MAX = 4;

    struct alignas(16) LightBufferData {
        int     enableLighting; // ライティング全体の有効・無効
        float   padding[3];

        GPU_DirectionalLight directionalLights[DIRECTIONAL_LIGHT_MAX];
        GPU_PointLight  pointLights[POINT_LIGHT_MAX];
        GPU_SpotLight   spotLights[SPOT_LIGHT_MAX];
    } m_lightBufferData;

    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;

    ComPtr<ID3D11Buffer> m_lightBuffer;

    int m_lightCount;

public:
    // 初期化
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    // 終了処理
    void Finalize() override;
    // 実行
    void Process(IScene* pScene) override;

    // ライトをセット
    void BindLightCB(bool enable);

    // DirectionalLightの方向を取得
    XMFLOAT3 GetDirectionalLightDirection() const {
        return {
            m_lightBufferData.directionalLights[0].direction.x,
            m_lightBufferData.directionalLights[0].direction.y,
            m_lightBufferData.directionalLights[0].direction.z
        };
    }

};

#endif // LIGHT_PROCESSOR_H