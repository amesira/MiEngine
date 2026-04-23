//---------------------------------------------------
// actor_factory.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//---------------------------------------------------
#ifndef ACTOR_FACTORY_H
#define ACTOR_FACTORY_H
#include <DirectXMath.h>
using namespace DirectX;

class GameObject;
class SceneBase;

namespace ActorFactory {
    /// @brief プレイヤー生成
    /// @param position 位置
    GameObject* CreatePlayer(SceneBase* scene, const XMFLOAT3& position);

};

#endif // !ACTOR_FACTORY_H