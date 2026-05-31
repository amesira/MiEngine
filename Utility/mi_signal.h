#pragma once
#include <DirectXMath.h>
using namespace DirectX;

namespace MiSignal
{
    // ランダムなグラデーションを生成する関数
    inline float RandomGradient(int x)
    {
        x = (x << 13) ^ x;
        return 1.0f - ((x * (x * x * 15731 + 789221) + 1376312589)
            & 0x7fffffff) / 1073741824.0f;
    }

    // フェード関数（滑らかな補間のためのイージング関数）
    inline float Fade(float t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    // 1D Perlinノイズを生成する関数
    inline float Perlin1D(float x)
    {
        int x0 = static_cast<int>(floorf(x));
        int x1 = x0 + 1;

        float t = x - x0;

        float g0 = RandomGradient(x0);
        float g1 = RandomGradient(x1);

        float v0 = g0 * (x - x0);
        float v1 = g1 * (x - x1);

        return v0 + (v1 - v0) * Fade(t);
    }
}