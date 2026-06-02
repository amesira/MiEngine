//---------------------------------------------------
// game_object_layer.h
//---------------------------------------------------
#ifndef GAME_OBJECT_LAYER_H
#define GAME_OBJECT_LAYER_H

#include <cstdint>

enum class RenderLayer {
    Default = 0,
    Player,
    Enemy,
    Particle,

    MAX,
};

using RenderLayerMask = uint32_t;

inline constexpr RenderLayerMask RenderLayerToMask(RenderLayer layer)
{
    return static_cast<RenderLayerMask>(1u << static_cast<uint32_t>(layer));
}

inline constexpr RenderLayerMask RENDER_LAYER_MASK_ALL =
    (static_cast<RenderLayerMask>(1u << static_cast<uint32_t>(RenderLayer::MAX))) - 1u;

enum class CollisionLayer {
    Default = 0,
    Field,
    Player,
    PlayerOnBall,
    Ball,
    Enemy,

    MAX,
};

#endif // GAME_OBJECT_LAYER_H
