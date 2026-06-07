//---------------------------------------------------
// render_effect_factory.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//---------------------------------------------------
#ifndef RENDER_EFFECT_FACTORY_H
#define RENDER_EFFECT_FACTORY_H
#include <DirectXMath.h>
using namespace DirectX;
#include <string>

class GameObject;
class IScene;
class SceneBase;

namespace RenderEffectFactory {
    // デカールエフェクト生成
    GameObject* CreateDecalEffect(SceneBase* scene, const XMFLOAT3& position, const std::wstring& decalTexturePath);
    GameObject* CreateParticleEffect(SceneBase* scene, const XMFLOAT3& position, const std::wstring& texturePath = L"asset\\Texture\\white.bmp");
    GameObject* CreateRunDustParticle(SceneBase* scene, std::string targetName, const std::wstring& texturePath = L"asset\\Texture\\white.bmp");
    GameObject* CreateChargeAbsorbParticle(SceneBase* scene, const XMFLOAT3& position, const std::wstring& texturePath = L"asset\\Texture\\white.bmp");
    GameObject* CreateHitEffect(IScene* scene, const XMFLOAT3& position);
    GameObject* CreateExplosionEffect(IScene* scene, const XMFLOAT3& position);

};

#endif // !RENDER_EFFECT_FACTORY_H
