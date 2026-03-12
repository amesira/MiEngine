//---------------------------------------------------
// scene_manager.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

class IScene;

class SceneManager {
public:
    enum class SceneID {
        None = 0,
        Title,
        Game,
        Result,
        Max
    };

private:
    IScene* m_pScene = nullptr;

    SceneID m_currentScene = SceneID::None;
    SceneID m_nextScene = SceneID::None;

public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();

    // シーンの切り替え
    void ChangeScene(SceneID sceneId);

    // 現在のシーンを取得
    IScene* GetCurrentScene() const { return m_pScene; }

private:
    // シーンの終了と破棄
    void ReleaseScene();

    // シーンの生成と初期化
    void LoadScene(SceneID sceneId);

public:
    // シーンのリロード
    void ReloadScene() {
        ReleaseScene();
        LoadScene(m_currentScene);
    }
};

#endif // SCENE_MANAGER_H