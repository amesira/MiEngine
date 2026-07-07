// projectile_factory.h
#pragma once
#include <DirectXMath.h>

#include <string>

using namespace DirectX;

class GameObject;
class IScene;
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

        const char* modelPath = "asset\\Model\\bullet.fbx";
        const char* materialName = "BulletHologramMaterial";
    };

    // ミサイル弾の生成に必要な情報をまとめた構造体
    struct MissileCreateDesc {
        XMFLOAT3 startPosition = {0.0f, 0.0f, 0.0f};
        XMFLOAT3 controlPoint1 = {0.0f, 0.0f, 0.0f};
        XMFLOAT3 controlPoint2 = { 0.0f, 0.0f, 0.0f };
        XMFLOAT3 targetPosition = { 0.0f, 0.0f, 0.0f };

        float radius = 0.5f;
        float duration = 1.0f;
        int layerMask = -1;

        const char* modelPath = "asset\\Model\\bullet.fbx";
        const char* materialName = "BulletHologramMaterial";
    };

    struct BezierLinePreviewCreateDesc {
        const char* name = "BezierLinePreview";
        float lineWidth = 0.08f;
        XMFLOAT4 lineColor = { 0.35f, 0.85f, 1.0f, 0.7f };
        int sampleCount = 24;
        bool visibleOnCreate = false;
    };

    // 弾の生成
    GameObject* CreateBullet(IScene* scene, const BulletCreateDesc& desc);

    // ミサイル弾の生成
    GameObject* CreateMissile(IScene* scene, const MissileCreateDesc& desc);
    GameObject* CreateBezierLinePreview(IScene* scene, const BezierLinePreviewCreateDesc& desc = {});
}
