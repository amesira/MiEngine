#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class GameObject;
class SceneBase;

namespace PrefabFactory
{
    // プレイヤープレハブ生成
    struct PlayerPrefab {
        GameObject* player;
        GameObject* runDustParticle;
        GameObject* chargeEffectParticle;
        GameObject* chargeLight;
    };
    PlayerPrefab CreatePlayerPrefab(SceneBase* scene, const XMFLOAT3& position);
}