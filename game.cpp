//===================================================
// game.cpp [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "game.h"

#include "factory.h"
#include "actor_factory.h"
#include "environment_factory.h"
#include "ui_factory.h"
#include "render_effect_factory.h"

#include "keyboard.h"

#include "mi_fps.h"
#include "scene_manager.h"

#include "text_component.h"
#include "image_component.h"
#include "camera_component.h"
#include "rigidbody_component.h"
#include "rect_transform_component.h"
#include "model_component.h"
#include "decal_component.h"
#include "transform_component.h"

#include "camera_control_behavior.h"

// ゲームシーン初期化処理
void GameScene::Initialize()
{
    this->Reset();

    // camera
    GameObject* camera = EnvironmentFactory::CreateCamera(this, { 0.0f,10.0f,-1.0f }, { 0.0f,0.0f,8.0f });
    EnvironmentFactory::AttachCameraControl(camera);

    GameObject* cube = this->CreateGameObject();
    Factory::CreateBox(cube, { 0.0f,-0.5f,10.0f }, {0.0f, 0.0f, 0.0f}, {40.0f, 1.0f, 40.0f}, {0.5f, 0.5f, 0.5f, 1.0f});
    cube->GetComponent<ModelComponent>()->SetEnable(false);

    cube = this->CreateGameObject();
    Factory::CreateBox(cube, { -3.0f,1.0f,10.0f }, { 0.0f, 0.0f, 10.0f }, { 1.0f, 1.0f, 1.0f }, { 0.7f, 0.5f, 0.5f, 1.0f });
    cube->AddComponent<RigidbodyComponent>();
    
    for (int i = 0; i < 5; i++) {
        cube = this->CreateGameObject();
        Factory::CreateBox(cube, { 5.0f,2.0f + i * 3.0f,10.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f + i * 0.2f, 0.2f, 0.2f, 1.0f });
        cube->AddComponent<RigidbodyComponent>();
    }
    
    // light
    EnvironmentFactory::CreateDirectionalLight(this, { 0.0f,-1.0f,0.5f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.8f,0.8f,0.8f,1.0f });
    EnvironmentFactory::CreatePointLight(this, { 1.0f, 1.0f, 0.0f, 1.0f }, 10.0f);

    // player
    GameObject* player = ActorFactory::CreatePlayer(this, { 0.0f,3.0f,10.0f });

    // ui
    GameObject* uiText = UiFactory::CreateUiText(this, u8"Hello, World!");
    UiFactory::SetupUiTransform(uiText, { 300.0f, 100.0f }, { 200.0f, 50.0f });

    GameObject* uiImage = UiFactory::CreateUiImage(this, L"asset\\Texture\\test.jpg");
    UiFactory::SetupUiTransform(uiImage, { 300.0f, 300.0f }, { 100.0f, 100.0f });

    GameObject* uiSlider = UiFactory::CreateUiSlider(this, { 0.2f, 0.2f, 0.2f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }, 0.5f);
    UiFactory::SetupUiTransform(uiSlider, { 300.0f, 500.0f }, { 200.0f, 20.0f });

    // model
    GameObject* model = this->CreateGameObject();
    Factory::CreateModel(model, "asset\\Model\\character.fbx", { 3.0f, 0.0f, 10.0f }, { 1.0f, 1.0f, 1.0f });

    // JointGroup
    GameObject* jointGroup = this->CreateGameObject();
    Factory::CreateJointGroup(jointGroup, {0.0f, 0.0f, 0.0f}, {10.0f, 1.0f, 0.0f}, 0.7f);

    // field
    {
        GameObject* field = this->CreateGameObject();
        Factory::CreateModel(field, "asset\\Model\\field.fbx", { 0.0f, -1.0f, 10.0f }, { 40.0f, 40.0f, 40.0f });
        field->GetComponent<ModelComponent>()->GetMaterialSlots()[0].isOverrideBaseColor = true;
        field->GetComponent<ModelComponent>()->GetMaterialSlots()[0].overrideBaseColor = { 0.5f, 0.5f, 0.5f, 1.0f };
    }

    // decal
    {
        GameObject* decal = RenderEffectFactory::CreateDecalEffect(this, {0.0f, 5.0f, 0.5f}, L"asset\\Texture\\test.jpg");
        
    }
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