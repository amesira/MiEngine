//----------------------------------------------------
// factory.cpp [ゲームオブジェクト工場]
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#include "factory.h"

// component
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/collider_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/rect_transform_component.h"
#include "Engine/Framework/Component/text_component.h"
#include "Engine/Framework/Component/image_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/light_component.h"
#include "Engine/Framework/Component/slider_component.h"
#include "Engine/Framework/Component/joint_component.h"
#include "Engine/Framework/Component/joint_group_component.h"
#include "Engine/Framework/Component/animation_component.h"

#include "Engine/engine_service_locator.h"
#define MATERIAL_REPOSITORY EngineServiceLocator::GetMaterialRepository()
#define MODEL_REPOSITORY EngineServiceLocator::GetModelRepository()
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

void Factory::CreateBox(GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color)
{
    cube->SetName("Box");

    // component生成・登録
    TransformComponent* transform = cube->AddComponent<TransformComponent>();
    BoxColliderComponent* collider = cube->AddComponent<BoxColliderComponent>();
    ModelComponent* modelComp = cube->AddComponent<ModelComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetEulerAngle(rotation);
    transform->SetScaling(scaling);
    collider->SetScale({
        scaling.x * 2.0f,
        scaling.y * 2.0f,
        scaling.z * 2.0f
        });

    ModelResource* modelResource = EngineServiceLocator::GetModelRepository()->GetModel("asset\\Model\\cube.fbx");
    modelComp->SetModelResource(modelResource);

    auto& materialSlots = modelComp->GetMaterialSlots();
    if (!materialSlots.empty()) {
        materialSlots[0].isOverrideBaseColor = true;
        materialSlots[0].overrideBaseColor = color;
    }
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

void Factory::CreateAnimationModel(GameObject* obj, const char* modelPath, XMFLOAT3 position, XMFLOAT3 scaling, const char* animationPath, float animationSpeed)
{
    obj->SetName("AnimationModel");

    TransformComponent* transform = obj->AddComponent<TransformComponent>();
    ModelComponent* modelComp = obj->AddComponent<ModelComponent>();
    AnimationComponent* animationComp = obj->AddComponent<AnimationComponent>();

    transform->SetPosition(position);
    transform->SetScaling(scaling);

    ModelResource* modelResource = EngineServiceLocator::GetModelRepository()->GetModel(modelPath);
    modelComp->SetModelResource(modelResource);

    const char* clipPath = animationPath != nullptr ? animationPath : modelPath;
    if (modelResource != nullptr && modelResource->animationClips.empty()) {
        MODEL_REPOSITORY->LoadAnimation(modelResource, clipPath);
    }

    animationComp->SetAnimationState(0, animationSpeed);
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
        jointModel->SetModelResource(MODEL_REPOSITORY->GetModel("asset\\Model\\sphere.fbx"));

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

void Factory::CreateField(GameObject* field, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color)
{
    field->SetName("Field");

    // component生成・登録
    TransformComponent* transform = field->AddComponent<TransformComponent>();
    BoxColliderComponent* collider = field->AddComponent<BoxColliderComponent>();
    ModelComponent* modelComp = field->AddComponent<ModelComponent>();

    // component設定
    transform->SetPosition(position);
    transform->SetEulerAngle(rotation);
    transform->SetScaling(scaling);
    collider->SetScale({
        scaling.x * 2.0f,
        scaling.y * 2.0f,
        scaling.z * 2.0f
        });

    // モデルはキューブを使用する
    ModelResource* modelResource = EngineServiceLocator::GetModelRepository()->GetModel("asset\\Model\\cube.fbx");
    modelComp->SetModelResource(modelResource);

    // テクスチャ設定
    MaterialResource* fieldMat = MATERIAL_REPOSITORY->GetMaterial("FieldMaterial");
    if (fieldMat) {
        fieldMat->shaderProgram = SHADER_REPOSITORY->GetShaderProgramResource(ShaderBase::Lit);
        fieldMat->baseColor = color;
        fieldMat->albedoTexture = TEXTURE_REPOSITORY->GetTextureResource(L"asset\\Texture\\TestField.png");
        fieldMat->uvTiling = { 40.0f, 40.0f };
    }
    auto& materialSlots = modelComp->GetMaterialSlots();
    if (!materialSlots.empty()) {
        materialSlots[0].materialResource = fieldMat;
    }
}
