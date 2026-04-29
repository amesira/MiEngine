//---------------------------------------------------
// engine.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef MI_ENGINE_H
#define MI_ENGINE_H
#include <windows.h>

#include "Engine/game_world.h"
#include "Engine/Editor/editor_manager.h"
#include "Engine/Manager/resource_manager.h"
#include "Engine/Manager/shader_manager.h"

class MiEngine {
private:
    bool m_isRunning = false;

    EditorManager   m_editorManager;
    EditorContext*  m_editorContext;

    ResourceManager m_resourceManager;
    ShaderManager   m_shaderManager;

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
    // ShaderManagerへのアクセス
    ShaderManager& GetShaderManager() { return m_shaderManager; }

    // EditorContextへのアクセス
    void AddLogMessage(const std::string& message) {
        // 最大ログ数制限
        const size_t MAX_LOG = 1024;
        if (m_editorContext->logMessages.size() > MAX_LOG){
            m_editorContext->logMessages.erase(m_editorContext->logMessages.begin());
        }
    }

};

#endif // MI_ENGINE_H