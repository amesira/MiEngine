#pragma once
class IScene;
struct RenderView;
class GameObject;

#include <string>
#include <vector>

struct EditorContext {
    IScene* scene = nullptr;                  // 観測対象のシーン
    RenderView* sceneRenderView = nullptr;   // SceneView表示用
    GameObject* selectedObject = nullptr;    // 現在選択中のオブジェクト

    // ディスプレイサイズ（SceneViewのサイズ計算などで使用）
    float displayX = 1280.0f; // ディスプレイの幅
    float displayY = 720.0f;  // ディスプレイの高さ

    // 各ウィンドウのサイズ（SceneViewのサイズ計算などで使用）
    float toolbarHeight = 40.0f;
    float hierarchyWidth = 300.0f;
    float inspectorWidth = 300.0f;

    std::vector<std::string> logMessages; // ログメッセージのリスト

    enum class EditorMode {
        Edit,
        Play,
    } currentEditorMode = EditorMode::Edit;
    bool triggerSceneReload = false; // シーンのリロード要求フラグ

    /*enum class ViewMode {
        SceneView,
        GameView,
        DebugView
    } currentViewMode = ViewMode::SceneView;*/
};