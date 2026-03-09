//===================================================
// physics_processor.cpp [物理演算制御プロセッサー]
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "physics_processor.h"
#include "game_object.h"
#include "scene_interface.h"

#include "mi_fps.h"

void PhysicsProcessor::Initialize()
{
    // 各Passの初期化
    m_integratePass.Initialize();
    m_collisionPass.Initialize();
    m_resolvePass.Initialize();
}

void PhysicsProcessor::Finalize()
{
    // 各Passの終了処理
    m_integratePass.Finalize();
    m_collisionPass.Finalize();
    m_resolvePass.Finalize();
}

void PhysicsProcessor::Process(IScene* pScene)
{
    // 各Passの処理
    m_integratePass.Process(pScene);
    m_collisionPass.Process(pScene);
    m_resolvePass.Process(pScene);
}
