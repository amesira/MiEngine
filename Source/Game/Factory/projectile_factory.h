// projectile_factory.h
#pragma once
#include <DirectXMath.h>

#include <string>

using namespace DirectX;

class GameObject;
class SceneBase;
class TextureResource;

namespace ProjectileFactory
{
    // 弾の生成に必要な情報をまとめた構造体
    struct BulletCreateDesc {
        XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
        XMFLOAT3 velocity = { 0.0f, 0.0f, 1.0f };
        float radius = 0.25f;
        float lifeTime = 3.0f;
        int layerMask = -1;

        const char* modelPath = "asset\\Model\\sphere.fbx";
        const char* materialName = "BulletHologramMaterial";
    };

    // 弾の生成
    GameObject* CreateBullet(SceneBase* scene, const BulletCreateDesc& desc);
}
