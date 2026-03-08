//---------------------------------------------------
// engine.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef MI_ENGINE_H
#define MI_ENGINE_H
#include <windows.h>

class MiEngine {
private:
    bool m_isRunning;

    // AssetManager* m_pAssetManager;
    // Renderer* m_pRenderer;
    // InputSystem* m_pInputSystem;
    // AudioSystem* m_pAudioSystem;

    // GameWorld* m_pGameWorld;
    // DebugWorld* m_pDebugWorld;

public:
    bool Initialize(HWND hWnd);
    void Finalize();
    bool RunOneFrame();

private:
    void Update();
    void Render();
};

#endif // MI_ENGINE_H