//===================================================
// game.cpp [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "game.h"

#include "Game/Factory/factory.h"
#include "Game/Factory/actor_factory.h"
#include "Game/Factory/environment_factory.h"
#include "Game/Factory/ui_factory.h"
#include "Game/Factory/render_effect_factory.h"
#include "Game/Factory/prefab_factory.h"
#include "Game/Factory/projectile_factory.h"

#include "Engine/Device/keyboard.h"
#include "Engine/Device/mi_fps.h"

#include "Engine/Manager/scene_manager.h"

#include "Engine/Framework/Component/text_component.h"
#include "Engine/Framework/Component/image_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/rect_transform_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/decal_component.h"
#include "Engine/Framework/Component/transform_component.h"

#include "Game/Behavior/camera_control_behavior.h"

#include "Game/ControllerBehavior/game_effect_controller.h"
#include "Game/ControllerBehavior/custom_post_effect_controller.h"

// ゲームシーン初期化処理
void GameScene::Initialize()
{
    this->Reset();

    // GameEffectControllerの生成
    GameObject* gameControllerObj = this->CreateGameObject();
    gameControllerObj->SetName("GameController");
    gameControllerObj->AddComponent<TransformComponent>();
    gameControllerObj->AddComponent<GameEffectController>();
    gameControllerObj->AddComponent<CustomPostEffectController>();

    // camera
    GameObject* camera = EnvironmentFactory::CreateCamera(this, { 0.0f,10.0f,-1.0f }, { 0.0f,0.0f,8.0f });
    EnvironmentFactory::AttachCameraControl(camera);
    camera->SetName("MainCamera");

    // light
    EnvironmentFactory::CreateDirectionalLight(this, { 0.0f,-1.0f,0.5f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.8f,0.8f,0.8f,1.0f });
    EnvironmentFactory::CreatePointLight(this, { 1.0f, 1.0f, 0.0f, 1.0f }, 10.0f);

    // Field
    GameObject* field = this->CreateGameObject();
    Factory::CreateField(field, { 0.0f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 20.0f, 1.0f, 20.0f }, { 0.2f, 0.2f, 0.2f, 1.0f });

    // プレイヤープレハブ生成
    PrefabFactory::PlayerPrefab playerPrefab = PrefabFactory::CreatePlayerPrefab(this, { 0.0f,3.0f,10.0f });

    // テスト：JointGroup
    GameObject* jointGroup = this->CreateGameObject();
    Factory::CreateJointGroup(jointGroup, {0.0f, 0.0f, 0.0f}, {10.0f, 1.0f, 0.0f}, 0.7f);

    // 敵生成
    for (int i = 0; i < 5; i++) {
        ActorFactory::CreateSimpleEnemy(this, { -5.0f + i * 2.5f, 0.0f, -5.0f });
    }

    // テスト：ParticleSystem
    //RenderEffectFactory::CreateParticleEffect(this, { 0.0f, 0.0f, 0.0f }, L"asset\\Texture\\particle.png");

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