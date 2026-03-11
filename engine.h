//---------------------------------------------------
// engine.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef MI_ENGINE_H
#define MI_ENGINE_H
#include <windows.h>

#include "game_world.h"
#include "mi_imgui_manager.h"

#include "editor_context.h"

class MiEngine {
public:
    static MiEngine& GetInstance() {
        static MiEngine instance;
        return instance;
    }

    bool Initialize(HWND hWnd);
    void Finalize();
    bool RunOneFrame();

private:
    void Update();
    void Render();

private:
    bool m_isRunning;

    EditorContext m_editorContext;

    MiImguiManager m_imguiManager;
    // AssetManager* m_pAssetManager;
    // Renderer* m_pRenderer;
    // InputSystem* m_pInputSystem;
    // AudioSystem* m_pAudioSystem;

    GameWorld m_gameWorld;
    // DebugWorld* m_pDebugWorld;

public:
    const GameWorld& GetGameWorld() const { return m_gameWorld; }

};

#endif // MI_ENGINE_H