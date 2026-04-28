#pragma once
#include <string>
#include <DirectXMath.h>
using namespace DirectX;

// スローモーションリクエスト構造体
struct SlowMotionRequest {
    bool trigger = false;
    float duration = 0.0f;
    float scale = 0.5f;
};

// エフェクトリクエスト構造体
struct EffectRequest {
    bool trigger = false;

    std::string effectName;
    XMFLOAT3 position;
    XMFLOAT3 rotation;
};

// ポストエフェクト（色相）リクエスト構造体
struct PostEffectColorRequest {
    bool trigger = false;
    float duration = 0.0f;

    std::string effectName = "ColorTint";
    XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    float intensity = 1.0f;
};

// ポストエフェクト（画面効果）リクエスト構造体
struct PostEffectScreenRequest {
    bool trigger = false;
    float duration = 0.0f;

    std::string effectName = "ScreenEffect";
    float intensity = 1.0f;
};

class PlayerVisualRequest {
public:
    SlowMotionRequest slowMotionRequest;
    EffectRequest effectRequest;
    PostEffectColorRequest postEffectColorRequest;
    PostEffectScreenRequest postEffectScreenRequest;

    void Clear() {
        slowMotionRequest.trigger = false;
        effectRequest.trigger = false;
        postEffectColorRequest.trigger = false;
        postEffectScreenRequest.trigger = false;
    }
};