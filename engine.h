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
#include "resource_manager.h"

class MiEngine {
private:
    bool m_isRunning = false;
    EditorContext m_editorContext;

    MiImguiManager m_imguiManager;
    ResourceManager m_resourceManager;
    // AssetManager* m_pAssetManager;
    // Renderer* m_pRenderer;
    // InputSystem* m_pInputSystem;
    // AudioSystem* m_pAudioSystem;

    GameWorld m_gameWorld;
    // DebugWorld* m_pDebugWorld;

public:
    MiEngine() {
        static bool instantiated = false;
        assert(!instantiated && "MiEngineは1つしかインスタンス化できません。");
        instantiated = true;
    }

    bool Initialize(HWND hWnd);
    void Finalize();
    bool RunOneFrame();

private:
    void Update();
    void Render();


public:
    // GameWorldへのアクセス
    const GameWorld& GetGameWorld() const { return m_gameWorld; }

    // ResourceManagerへのアクセス
    ResourceManager& GetResourceManager() { return m_resourceManager; }

    // EditorContextへのアクセス
    void AddLogMessage(const std::string& message) {
        m_editorContext.logMessages.push_back(message);

        // 最大ログ数制限
        const size_t MAX_LOG = 1024;
        if (m_editorContext.logMessages.size() > MAX_LOG){
            m_editorContext.logMessages.erase(
                m_editorContext.logMessages.begin());
        }
    }

};

#endif // MI_ENGINE_H