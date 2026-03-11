#pragma once
class IScene;
struct RenderView;
class GameObject;

struct EditorContext {
    const IScene* scene = nullptr;                  // 観測対象のシーン
    const RenderView* sceneRenderView = nullptr;   // SceneView表示用
    const GameObject* selectedObject = nullptr;    // 現在選択中のオブジェクト

    float displayX = 1280.0f; // ディスプレイの幅
    float displayY = 720.0f;  // ディスプレイの高さ

    float toolbarHeight = 40.0f;
    float hierarchyWidth = 300.0f;
    float inspectorWidth = 300.0f;
};