//===================================================
// environment_factory.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//===================================================
#include "environment_factory.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_base.h"

// component
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/light_component.h"

// behavior
#include "Game/Behavior/camera_control_behavior.h"

#include "Engine/engine_service_locator.h"

// ----------------------------------------------------------- Camera

// カメラ生成
GameObject* EnvironmentFactory::CreateCamera(SceneBase* scene, const XMFLOAT3& position, const XMFLOAT3& atPosition)
{
    GameObject* camera = scene->CreateGameObject();
    camera->SetName("Camera");

    // component生成・登録
    TransformComponent* transform = camera->AddComponent<TransformComponent>();
    CameraComponent* cameraComp = camera->AddComponent<CameraComponent>();
    transform->SetPosition(position);
    cameraComp->SetAtPosition(atPosition);
    cameraComp->SetFov(60.0f);
    cameraComp->SetAspect(16.0f / 9.0f);
    cameraComp->SetNearClip(0.1f);
    cameraComp->SetFarClip(100.0f);

    return camera;
}

// CameraControlBehaviorをアタッチ
bool EnvironmentFactory::AttachCameraControl(GameObject* camera)
{
    if (!camera) return false;

    CameraControlBehavior* controlBehavior = camera->AddComponent<CameraControlBehavior>();

    return true;
}

// ----------------------------------------------------------- Light

// DirectionalLight生成
GameObject* EnvironmentFactory::CreateDirectionalLight(SceneBase* scene, const XMFLOAT4& direction, const XMFLOAT4& diffuse, const XMFLOAT4& ambient)
{
    GameObject* obj = scene->CreateGameObject();
    obj->SetName("DirectionalLight");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    LightComponent* lightComp = obj->AddComponent<LightComponent>();

    // component設定
    lightComp->SetLightType(LightComponent::LightType::Directional);

    lightComp->SetDirection(direction);
    lightComp->SetDiffuse(diffuse);
    lightComp->SetAmbient(ambient);

    lightComp->SetIntensity(0.6f);

    return obj;
}

// PointLight生成
GameObject* EnvironmentFactory::CreatePointLight(SceneBase* scene, const XMFLOAT4& diffuse, float range)
{
    GameObject* obj = scene->CreateGameObject();
    obj->SetName("PointLight");

    // component生成・登録
    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    LightComponent* lightComp = obj->AddComponent<LightComponent>();

    // component設定
    lightComp->SetLightType(LightComponent::LightType::Point);
    lightComp->SetDirection({ 0.0f, -1.0f, 0.0f, 0.0f });
    lightComp->SetDiffuse(diffuse);

    lightComp->SetIntensity(2.0f);
    lightComp->SetRange(range);


    SphereColliderComponent* collider = obj->AddComponent<SphereColliderComponent>();
    collider->SetRadius(0.1f);
    collider->SetCreateCorrection(false);

    return obj;
}
