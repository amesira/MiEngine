//---------------------------------------------------
// game_world.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef GAME_WORLD_H
#define GAME_WORLD_H

class SceneManager;

class PhysicsProcessor;
class CameraProcessor;
class LightProcessor;
class RenderProcessor;

class GameWorld {
public:
    void Initialize();
    void Finalize();
    void Update();
    void Render();

private:
    // Scene管理
    SceneManager*   m_pSceneManager;

    // Processor群
    PhysicsProcessor*   m_pPhysicsProcessor;
    CameraProcessor*    m_pCameraProcessor;
    LightProcessor*     m_pLightProcessor;
    RenderProcessor*    m_pRenderProcessor;

};

#endif // GAME_WORLD_H