//===================================================
// scene_manager.cpp [シーン管理]
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "scene_manager.h"
#include "scene_interface.h"

#include "game.h"

// シーン管理の初期化
void SceneManager::Initialize()
{
    m_currentScene = SceneID::Game;
    m_nextScene = m_currentScene;

    // 最初のシーンをセット
    LoadScene(m_currentScene);
}

// シーン管理の終了処理
void SceneManager::Finalize()
{
    ReleaseScene(); // 現在のシーンの終了と破棄
}

// シーン管理の更新処理
void SceneManager::Update()
{
    // シーン切り替え
    if (m_currentScene != m_nextScene) {
        ReleaseScene();         // 現在のシーンの終了と破棄
        LoadScene(m_nextScene); // 次のシーンの生成と初期化

        m_currentScene = m_nextScene;
    }

    // 現在のシーンの更新
    if (m_pScene) {
        m_pScene->Update();
    }
}

// シーン管理の描画処理
void SceneManager::Draw()
{
    // 現在のシーンの描画
    if (m_pScene) {
        m_pScene->Draw();
    }
}

void SceneManager::ChangeScene(SceneID sceneId)
{
    // 次のシーンをセット
    m_nextScene = sceneId;
}

//--------------------------------- private
// シーンの終了と破棄
void SceneManager::ReleaseScene()
{
    if (m_pScene) {
        m_pScene->Finalize(); // シーンの終了処理
        delete m_pScene;      // シーンの破棄
        m_pScene = nullptr;
    }
}

// シーンの生成と初期化
void SceneManager::LoadScene(SceneID sceneId)
{
    // シーンIDに応じてシーンを生成
    switch (sceneId) {
        case SceneID::Title:
            // m_pScene = new TitleScene();
            break;
        case SceneID::Game:
            m_pScene = new GameScene();
            break;
        case SceneID::Result:
            // m_pScene = new ResultScene();
            break;
        default:
            m_pScene = nullptr;
            break;
    };

    // シーンの初期化
    if (m_pScene) {
        m_pScene->Initialize();
    }
}
