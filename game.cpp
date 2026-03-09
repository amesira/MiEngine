//===================================================
// game.cpp [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "game.h"

#include "processor_manager.h"

#include "factory.h"
#include "keyboard.h"
#include "text_component.h"
#include "image_component.h"
#include "camera_component.h"

#include "rect_transform_component.h"

#include "mi_fps.h"
#include "scene_manager.h"

// ゲームシーン初期化処理
void GameScene::Initialize()
{
    this->Reset();

    // camera
    GameObject* camera = this->CreateGameObject();
    Factory::CreateCamera(camera, { 0.0f,15.0f,-1.0f }, { 0.0f,0.0f,8.0f });

    GameObject* cube = this->CreateGameObject();
    Factory::CreateBox(cube, { 0.0f,0.0f,10.0f }, {0.0f, 0.0f, 0.0f}, {6.0f, 1.0f, 6.0f}, {0.5f, 0.5f, 0.5f, 1.0f});
    
    // light
    GameObject* light = this->CreateGameObject();
    Factory::CreateLight(light, { 0.5f,-1.0f,0.5f,0.0f }, { 0.1f,0.1f,0.1f,1.0f }, { 0.7f,0.7f,0.7f,1.0f });

    // ui


}

// ゲームシーン終了処理
void GameScene::Finalize()
{
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Destroy();
    }
}

// ゲームシーン更新処理
void GameScene::Update()
{
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Update();
    }

    // 破棄予約されたGameObjectの収集
    this->CollectDestroyedGameObjects();
}

// ゲームシーン描画処理
void GameScene::Draw()
{
    
}