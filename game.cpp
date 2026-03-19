//===================================================
// game.cpp [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "game.h"

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
    Factory::CreateCamera(camera, { 0.0f,10.0f,-1.0f }, { 0.0f,0.0f,8.0f });

    GameObject* cube = this->CreateGameObject();
    Factory::CreateBox(cube, { 0.0f,0.0f,10.0f }, {0.0f, 0.0f, 0.0f}, {6.0f, 1.0f, 6.0f}, {0.5f, 0.5f, 0.5f, 1.0f});
    cube = this->CreateGameObject();
    Factory::CreateBox(cube, { -3.0f,1.0f,10.0f }, { 0.0f, 0.0f, 10.0f }, { 1.0f, 1.0f, 1.0f }, { 0.7f, 0.5f, 0.5f, 1.0f });
    
    // light
    GameObject* light = this->CreateGameObject();
    Factory::CreateDirectionalLight(light, { -0.5f,-1.0f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.8f,0.8f,0.8f,1.0f });
    light = this->CreateGameObject();
    Factory::CreatePointLight(light, {1.0f, 1.0f, 0.0f, 1.0f}, 10.0f);

    // player
    GameObject* player = this->CreateGameObject();
    Factory::CreatePlayer(player, { 0.0f,3.0f,10.0f });

    // ui
    GameObject* uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 300.0f, 100.0f }, { 200.0f, 50.0f }, u8"Hello, World!");

    GameObject* uiImage = this->CreateGameObject();
    Factory::CreateUiImage(uiImage, { 300.0f, 300.0f }, { 100.0f, 100.0f }, L"asset\\Texture\\test.jpg");

    GameObject* uiSlider = this->CreateGameObject();
    Factory::CreateUiSlider(uiSlider, { 300.0f, 500.0f }, { 200.0f, 20.0f }, 0.5f);

    // model
    GameObject* model = this->CreateGameObject();
    Factory::CreateModel(model, "asset\\Model\\character.fbx", { 3.0f, 0.0f, 10.0f }, { 1.0f, 1.0f, 1.0f });

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
    // 破棄予約されたGameObjectの収集
    this->CollectDestroyedGameObjects();
}

// ゲームシーン描画処理
void GameScene::Draw()
{
    
}