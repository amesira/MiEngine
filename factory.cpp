//----------------------------------------------------
// factory.cpp [ゲームオブジェクト工場]
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#include "factory.h"

// component
#include "transform_component.h"
#include "collider_component.h"
#include "rigidbody_component.h"
#include "rect_transform_component.h"
#include "text_component.h"
#include "image_component.h"
#include "model_component.h"
#include "camera_component.h"
#include "light_component.h"
#include "slider_component.h"

// behavior
#include "player_behavior.h"

void Factory::CreateCamera(GameObject* obj, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 atPosition)
{
    obj->SetName("Camera");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    CameraComponent* camera = obj->AddComponent<CameraComponent>();
    
    // component設定
    transform->SetPosition(position);

    camera->SetAtPosition(atPosition);
    camera->SetFov(60.0f);
    camera->SetAspect(16.0f / 9.0f);
    camera->SetNearClip(0.1f);
    camera->SetFarClip(100.0f);
}

void Factory::CreateLight(GameObject* obj, DirectX::XMFLOAT4 direction, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 ambient)
{
    obj->SetName("Light");

    // component生成・登録
    LightComponent* lightComp = obj->AddComponent<LightComponent>();
    
    // component設定
    lightComp->SetDirection(direction);
    lightComp->SetDiffuse(diffuse);
    lightComp->SetAmbient(ambient);
}

void Factory::CreateBox(GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color)
{
    cube->SetName("Box");

    // component生成・登録
    TransformComponent* transform = cube->AddComponent<TransformComponent>();
    BoxColliderComponent* collider = cube->AddComponent<BoxColliderComponent>();
    ModelComponent* modelComp = cube->AddComponent<ModelComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetEulerRotation(rotation);
    transform->SetScaling(scaling);
    collider->SetScale({
        scaling.x * 2.0f,
        scaling.y * 2.0f,
        scaling.z * 2.0f
        });
    modelComp->LoadModel("asset\\Model\\cube.fbx");
    modelComp->SetColor(color);
}

void Factory::CreatePlayer(GameObject* player, DirectX::XMFLOAT3 position)
{
    player->SetName("Player");

    // component生成・登録
    TransformComponent* transform = player->AddComponent<TransformComponent>();
    BoxColliderComponent* collider = player->AddComponent<BoxColliderComponent>();
    ModelComponent* modelComp = player->AddComponent<ModelComponent>();
    RigidbodyComponent* rigidbody = player->AddComponent<RigidbodyComponent>();

    // component設定
    transform->SetPosition(position);

    XMFLOAT3 scaling = { 1.0f, 1.0f, 1.0f };
    transform->SetScaling(scaling);
    collider->SetScale({
        scaling.x * 2.0f,
        scaling.y * 2.0f,
        scaling.z * 2.0f
        });
    modelComp->LoadModel("asset\\Model\\cube.fbx");
    modelComp->SetColor({1.0f, 1.0f, 0.0f, 1.0f});

    // behavior生成・登録
    player->AddComponent<PlayerBehavior>();
}

void Factory::CreateUiImage(GameObject* uiImage, XMFLOAT2 position, XMFLOAT2 size, const wchar_t* texturePath)
{
    uiImage->SetName("UiImage");

    // component生成・登録
    RectTransformComponent* rectTransform = uiImage->AddComponent<RectTransformComponent>();
    ImageComponent* imageComp = uiImage->AddComponent<ImageComponent>();

    // component設定
    rectTransform->SetPosition({ position.x, position.y, 0.0f });
    rectTransform->SetScaling({ size.x, size.y, 1.0f });
    imageComp->Load(texturePath);
}

void Factory::CreateUiText(GameObject* uiText, XMFLOAT2 position, XMFLOAT2 size, const std::u8string text)
{
    uiText->SetName("UiText");

    // component生成・登録
    RectTransformComponent* rectTransform = uiText->AddComponent<RectTransformComponent>();
    TextComponent* textComp = uiText->AddComponent<TextComponent>();

    // component設定
    rectTransform->SetPosition({ position.x, position.y, 0.0f });
    rectTransform->SetScaling({ size.x, size.y, 1.0f });
    textComp->SetText(text);
    textComp->SetFontSize(32.0f);
    textComp->SetColor({1.0f, 0.5f, 0.5f, 1.0f});
    textComp->SetCenter(true);
}

void Factory::CreateUiSlider(GameObject* uiSlider, XMFLOAT2 position, XMFLOAT2 size, float value)
{
    uiSlider->SetName("UiSlider");

    // component生成・登録
    RectTransformComponent* rectTransform = uiSlider->AddComponent<RectTransformComponent>();
    SliderComponent* sliderComp = uiSlider->AddComponent<SliderComponent>();

    // component設定
    rectTransform->SetPosition({ position.x, position.y, 0.0f });
    rectTransform->SetScaling({ size.x, size.y, 1.0f });
    sliderComp->SetValue(value);
    sliderComp->SetBgColor({ 0.2f, 0.2f, 0.2f, 1.0f });
    sliderComp->SetFillColor({ 0.8f, 0.8f, 0.8f, 1.0f });
}
