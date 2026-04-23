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
class SceneBase;

namespace RenderEffectFactory {
    // デカールエフェクト生成
    GameObject* CreateDecalEffect(SceneBase* scene, const XMFLOAT3& position, const std::wstring& decalTexturePath);

};

#endif // !RENDER_EFFECT_FACTORY_H