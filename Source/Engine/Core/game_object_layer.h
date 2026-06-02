//---------------------------------------------------
// game_object_layer.h
//---------------------------------------------------
#ifndef GAME_OBJECT_LAYER_H
#define GAME_OBJECT_LAYER_H

enum class RenderLayer {
    Default = 0,
    Player,
    Enemy,
    Particle,

    MAX,
};

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
