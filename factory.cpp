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
#include "joint_component.h"
#include "joint_group_component.h"

// behavior
#include "player_behavior.h"

#include "engine_service_locator.h"

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

void Factory::CreateDirectionalLight(GameObject* obj, DirectX::XMFLOAT4 direction, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 ambient)
{
    obj->SetName("DirectionalLight");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    LightComponent* lightComp = obj->AddComponent<LightComponent>();
    
    // component設定
    lightComp->SetLightType(LightComponent::LightType::Directional);

    lightComp->SetDirection(direction);
    lightComp->SetDiffuse(diffuse);
    lightComp->SetAmbient(ambient);

    lightComp->SetIntensity(1.2f);
}

void Factory::CreatePointLight(GameObject* obj, XMFLOAT4 diffuse, float range)
{
    obj->SetName("PointLight");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    LightComponent* lightComp = obj->AddComponent<LightComponent>();

    // component設定
    lightComp->SetLightType(LightComponent::LightType::Point);

    lightComp->SetDiffuse(diffuse);

    lightComp->SetIntensity(2.0f);
    lightComp->SetRange(range);


    SphereColliderComponent* collider = obj->AddComponent<SphereColliderComponent>();
    collider->SetRadius(0.1f);
    collider->SetCreateCorrection(false);

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

    ModelResource* modelResource = EngineServiceLocator::GetModelRepository()->GetModel("asset\\Model\\cube.fbx");
    modelComp->SetModelResource(modelResource);
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

    ModelResource* modelResource = EngineServiceLocator::GetModelRepository()->GetModel("asset\\Model\\cube.fbx");
    modelComp->SetModelResource(modelResource);
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

void Factory::CreateModel(GameObject* obj, const char* modelPath, XMFLOAT3 position, XMFLOAT3 scaling)
{
    obj->SetName("Model");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    ModelComponent* modelComp = obj->AddComponent<ModelComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetScaling(scaling);
    ModelResource* modelResource = EngineServiceLocator::GetModelRepository()->GetModel(modelPath);
    modelComp->SetModelResource(modelResource);
}

void Factory::CreateJointGroup(GameObject* jointGroup, XMFLOAT3 startPosition, XMFLOAT3 endPosition, float interval)
{
    jointGroup->SetName("JointGroup");

    // component生成・登録
    TransformComponent* transform = jointGroup->AddComponent<TransformComponent>();
    JointGroupComponent* jointGroupComp = jointGroup->AddComponent<JointGroupComponent>();

    // component設定
    transform->SetPosition(startPosition);

    // JointComponentをintervalの間隔で配置する
    XMFLOAT3 direction = {
        endPosition.x - startPosition.x,
        endPosition.y - startPosition.y,
        endPosition.z - startPosition.z
    };

    float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    int jointCount = static_cast<int>(length / interval) + 1;

    unsigned int prevJointID = -1;

    for (int i = 0; i < jointCount; i++) {
        GameObject* jointObj = jointGroup->GetScene()->CreateGameObject();
        jointObj->SetName("Joint" + std::to_string(i));

        TransformComponent* jointTransform = jointObj->AddComponent<TransformComponent>();
        RigidbodyComponent* jointRigidbody = jointObj->AddComponent<RigidbodyComponent>();
        SphereColliderComponent* jointCollider = jointObj->AddComponent<SphereColliderComponent>();
        ModelComponent* jointModel = jointObj->AddComponent<ModelComponent>();
        JointComponent* jointComp = jointObj->AddComponent<JointComponent>();
        jointComp->SetGroupID(jointGroup->GetID());
        jointComp->SetRestLength(interval);

        jointTransform->SetScaling({0.3f, 0.3f, 0.3f});
        jointCollider->SetRadius(0.3f);

        if (prevJointID != -1) {
            jointComp->SetConnectedBodyID(prevJointID);
        }
        prevJointID = jointObj->GetID();

        float t = static_cast<float>(i) / (jointCount - 1);
        XMFLOAT3 jointPos = {
            startPosition.x + direction.x * t,
            startPosition.y + direction.y * t,
            startPosition.z + direction.z * t
        };

        jointTransform->SetPosition(jointPos);
        jointModel->SetModelResource(EngineServiceLocator::GetModelRepository()->GetModel("asset\\Model\\sphere.fbx"));
        jointModel->SetColor({ 0.5f, 0.5f, 1.0f, 1.0f });

        jointRigidbody->SetMass(3.0f);
        if (i == 0 || i == jointCount - 1) {
            // 最初と最後のJointはキネマティックにする
            jointRigidbody->SetIsKinematic(true);
        }
        else {
            jointRigidbody->SetIsKinematic(false);
            jointRigidbody->SetMass(3.0f);
        }
    }
}
