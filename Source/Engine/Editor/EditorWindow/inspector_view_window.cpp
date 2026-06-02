//===================================================
// inspector_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "inspector_view_window.h"

#include "Engine/engine.h"
#include "Engine/Editor/editor_context.h"
#include "Engine/render_view.h"

#include "Engine/Core/game_object.h"
#include "Engine/Framework/Component/behavior_component.h"

#include "Utility/mi_string.h"
#include "Engine/engine_service_locator.h"

// 主要なComponentのヘッダをインクルード
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rect_transform_component.h"
#include "Engine/Framework/Component/camera_component.h"
#include "Engine/Framework/Component/light_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"
#include "Engine/Framework/Component/collider_component.h"
#include "Engine/Framework/Component/text_component.h"
#include "Engine/Framework/Component/image_component.h"
#include "Engine/Framework/Component/slider_component.h"
#include "Engine/Framework/Component/model_component.h"
#include "Engine/Framework/Component/joint_group_component.h"
#include "Engine/Framework/Component/joint_component.h"
#include "Engine/Framework/Component/decal_component.h"
#include "Engine/Framework/Component/particle_system_component.h"

#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()
#define SHADER_REPOSITORY EngineServiceLocator::GetShaderRepository()

namespace
{
    void DrawMinMaxFloat(const char* label, ParticleSystemComponent::MinMaxFloat& value)
    {
        if (ImGui::TreeNode(label)) {
            ImGui::Checkbox("Random Between Two Constants", &value.randomBetweenTwoConstants);
            if (value.randomBetweenTwoConstants) {
                ImGui::DragFloat("Min", &value.constantMin, 0.01f);
                ImGui::DragFloat("Max", &value.constantMax, 0.01f);
            }
            else {
                ImGui::DragFloat("Constant", &value.constant, 0.01f);
            }
            ImGui::TreePop();
        }
    }

    void DrawMinMaxColor(const char* label, ParticleSystemComponent::MinMaxColor& value)
    {
        if (ImGui::TreeNode(label)) {
            ImGui::Checkbox("Random Between Two Colors", &value.randomBetweenTwoColors);
            if (value.randomBetweenTwoColors) {
                ImGui::ColorEdit4("Min", &value.colorMin.x);
                ImGui::ColorEdit4("Max", &value.colorMax.x);
            }
            else {
                ImGui::ColorEdit4("Color", &value.color.x);
            }
            ImGui::TreePop();
        }
    }

    void DrawFloatCurve(const char* label, ParticleSystemComponent::FloatCurve& curve)
    {
        if (ImGui::TreeNode(label)) {
            for (int i = 0; i < static_cast<int>(curve.keys.size()); ++i) {
                ImGui::PushID(i);
                ImGui::DragFloat("Time", &curve.keys[i].time, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Value", &curve.keys[i].value, 0.01f);
                if (ImGui::Button("Remove")) {
                    curve.keys.erase(curve.keys.begin() + i);
                    ImGui::PopID();
                    break;
                }
                ImGui::Separator();
                ImGui::PopID();
            }
            if (ImGui::Button("Add Key")) {
                curve.keys.push_back({ 1.0f, 1.0f });
            }
            ImGui::TreePop();
        }
    }
}

void InspectorViewWindow::Draw()
{
    ImGui::Text("Selected Object: %s", m_editorContext->selectedObject ? m_editorContext->selectedObject->GetName().c_str() : "None");
    if (m_editorContext->selectedObject) {
        auto components = m_editorContext->selectedObject->GetAllComponents();
        auto behaviorComponents = m_editorContext->selectedObject->GetBehaviorComponents();

        // 全コンポーネントを列挙
        ImGui::BeginChild("Component List", ImVec2(0, 100), true);
        for (auto* comp : components) {
            ImGui::Text("- %s", typeid(*comp).name());
        }
        ImGui::EndChild();

        ImGui::Separator();

        // 基本情報の表示
        {
            ImGui::BeginChild("Basic Info", ImVec2(0, 80), true);
            ImGui::Text("Name: %s", m_editorContext->selectedObject->GetName().c_str());
            ImGui::Text("ID: %d", m_editorContext->selectedObject->GetID());
            bool active = m_editorContext->selectedObject->GetActive();
            if (ImGui::Checkbox("Active", &active)) {
                m_editorContext->selectedObject->SetActive(active);
            }
            ImGui::EndChild();
        }
        
        ImGui::Separator();

        // GameObjectを引き渡し、各主要Componentのプロパティを表示
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.18f, 0.23f, 1.0f));
            ImGui::BeginChild("Component Inspector", ImVec2(0.0f, m_componentGroupSize.y), false);

            // コンテンツの描画
            ImGui::BeginGroup();
            DrawComponentInspector(m_editorContext->selectedObject);
            ImGui::EndGroup();

            // サイズ測定
            m_componentGroupSize = ImGui::GetItemRectSize();

            ImGui::EndChild();
            ImGui::PopStyleColor();
        }

        ImGui::Separator();

        // BehaviorComponentのプロパティ表示
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.23f, 0.18f, 1.0f));
            ImGui::BeginChild("Behavior Component Inspector", ImVec2(0.0f, m_behaviorGroupSize.y + 300.0f), false);

            // コンテンツの描画
            ImGui::BeginGroup();
            for (auto* behavior : behaviorComponents) {
                behavior->DrawComponentInspector();
            }
            ImGui::EndGroup();

            // サイズ測定
            m_behaviorGroupSize = ImGui::GetItemRectSize();

            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
    }
}

// GameObjectIDを引き渡し、各主要Componentのプロパティを表示
void InspectorViewWindow::DrawComponentInspector(GameObject* gameObject)
{
    if (!gameObject) return;

    // TransformComponentのプロパティ表示
    auto* transform = gameObject->GetComponent<TransformComponent>();
    if (transform) {
        ImGui::PushID(transform);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto position = transform->GetPosition();
            if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
                transform->SetPosition(position);
            }
            XMFLOAT3 eulerRotation = transform->GetEulerAngle();
            eulerRotation = MiMath::Multiply(eulerRotation, 360.0f / XM_2PI);
            if (ImGui::DragFloat3("Rotation", &eulerRotation.x, 0.1f)) {
                eulerRotation = MiMath::Multiply(eulerRotation, XM_2PI / 360.0f);
                transform->SetEulerAngle(eulerRotation);
            }
            auto scaling = transform->GetScaling();
            if (ImGui::DragFloat3("Scaling", &scaling.x, 0.1f)) {
                transform->SetScaling(scaling);
            }
        }
        ImGui::PopID();
    }
    // RectTransformComponentのプロパティ表示
    auto* rectTransform = gameObject->GetComponent<RectTransformComponent>();
    if (rectTransform) {
        ImGui::PushID(rectTransform);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Rect Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto position = rectTransform->GetPosition();
            if (ImGui::DragFloat3("Position", &position.x, 0.5f)) {
                rectTransform->SetPosition(position);
            }
            auto eulerRotation = rectTransform->GetRotation();
            if (ImGui::DragFloat3("Rotation", &eulerRotation.x, 0.1f)) {
                rectTransform->SetRotation(eulerRotation);
            }
            auto scaling = rectTransform->GetScaling();
            if (ImGui::DragFloat3("Scaling", &scaling.x, 0.5f)) {
                rectTransform->SetScaling(scaling);
            }
        }
        ImGui::PopID();
    }

    // LightComponentのプロパティ表示
    auto* light = gameObject->GetComponent<LightComponent>();
    if (light) {
        ImGui::PushID(light);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            int lightType = static_cast<int>(light->GetLightType());
            const char* lightTypeItems[] = { "Directional", "Point", "Spot" };
            if (ImGui::Combo("Light Type", &lightType, lightTypeItems, IM_ARRAYSIZE(lightTypeItems))) {
                light->SetLightType(static_cast<LightComponent::LightType>(lightType));
            }

            ImGui::Separator();

            // LightType
            switch (light->GetLightType()) {
            case LightComponent::LightType::Directional:
            {
                auto direction = light->GetDirection();
                if (ImGui::DragFloat4("Direction", &direction.x, 0.1f)) {
                    light->SetDirection(direction);
                }
                auto ambient = light->GetAmbient();
                if (ImGui::ColorEdit4("Ambient", &ambient.x)) {
                    light->SetAmbient(ambient);
                }
                break;
            }
            }

            auto diffuse = light->GetDiffuse();
            if (ImGui::ColorEdit4("Diffuse", &diffuse.x)) {
                light->SetDiffuse(diffuse);
            }

            ImGui::Separator();
            
            float intensity = light->GetIntensity();
            if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 10.0f)) {
                light->SetIntensity(intensity);
            }
            // Rangeは点光源・スポットライトのみ
            if (light->GetLightType() != LightComponent::LightType::Directional) {
                float range = light->GetRange();
                if (ImGui::DragFloat("Range", &range, 0.1f, 0.0f, 100.0f)) {
                    light->SetRange(range);
                }
            }
            // スポットライトのみスポット角度を表示
            if (light->GetLightType() == LightComponent::LightType::Spot) {
                float spotAngle = light->GetSpotAngle();
                if (ImGui::DragFloat("Spot Angle", &spotAngle, 0.1f, 1.0f, 179.0f)) {
                    light->SetSpotAngle(spotAngle);
                }
            }
        }
        ImGui::PopID();
    }

    // CameraComponentのプロパティ表示
    auto* camera = gameObject->GetComponent<CameraComponent>();
    if (camera) {
        ImGui::PushID(camera);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            XMFLOAT3 atPosition = camera->GetAtPosition();
            if (ImGui::DragFloat3("At Position", &atPosition.x, 0.1f)) {
                camera->SetAtPosition(atPosition);
            }
            float fov = camera->GetFov();
            if (ImGui::DragFloat("Field Of View", &fov, 0.1f, 1.0f, 179.0f)) {
                camera->SetFov(fov);
            }
            float nearClip = camera->GetNearClip();
            if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.01f, 1000.0f)) {
                camera->SetNearClip(nearClip);
            }
            float farClip = camera->GetFarClip();
            if (ImGui::DragFloat("Far Clip", &farClip, 0.1f, 0.01f, 10000.0f)) {
                camera->SetFarClip(farClip);
            }
        }
        ImGui::PopID();
    }

    // RigidbodyComponentのプロパティ表示
    auto* rigidbody = gameObject->GetComponent<RigidbodyComponent>();
    if (rigidbody) {
        if (BeginComponentSection(rigidbody, "Rigidbody")) {
            
            bool isKinematic = rigidbody->GetIsKinematic();
            if (ImGui::Checkbox("Is Kinematic", &isKinematic)) {
                rigidbody->SetIsKinematic(isKinematic);
            }

            auto velocity = rigidbody->GetVelocity();
            if (ImGui::DragFloat3("Velocity", &velocity.x, 0.1f)) {
                rigidbody->SetVelocity(velocity);
            }
            float mass = rigidbody->GetMass();
            if (ImGui::DragFloat("Mass", &mass, 0.1f)) {
                rigidbody->SetMass(mass);
            }
        }

        EndComponentSection();
    }

    // ColliderComponentのプロパティ表示
    auto* boxCollider = gameObject->GetComponent<BoxColliderComponent>();
    if (boxCollider) {
        if (BeginComponentSection(boxCollider, "Box Collider")) {
            auto center = boxCollider->GetCenter();
            if (ImGui::DragFloat3("Center", &center.x, 0.1f)) {
                boxCollider->SetCenter(center);
            }
            auto scale = boxCollider->GetScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
                boxCollider->SetScale(scale);
            }
        }

        EndComponentSection();
    }
    
    auto* sphereCollider = gameObject->GetComponent<SphereColliderComponent>();
    if (sphereCollider) {
        if (BeginComponentSection(sphereCollider, "Sphere Collider")) {
            
            auto center = sphereCollider->GetCenter();
            if (ImGui::DragFloat3("Center", &center.x, 0.1f)) {
                sphereCollider->SetCenter(center);
            }
            float radius = sphereCollider->GetRadius();
            if (ImGui::DragFloat("Radius", &radius, 0.1f)) {
                sphereCollider->SetRadius(radius);
            }
        }

        EndComponentSection();
    }

    // JointGroupComponentのプロパティ表示
    auto* jointGroup = gameObject->GetComponent<JointGroupComponent>();
    if (jointGroup) {
        ImGui::PushID(jointGroup);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Joint Group", ImGuiTreeNodeFlags_DefaultOpen)) {
            float stiffness = jointGroup->GetStiffness();
            if (ImGui::DragFloat("Stiffness", &stiffness, 0.1f, 0.0f, 1000.0f)) {
                jointGroup->SetStiffness(stiffness);
            }
            float damping = jointGroup->GetDamping();
            if (ImGui::DragFloat("Damping", &damping, 0.01f, 0.0f, 1.0f)) {
                jointGroup->SetDamping(damping);
            }

            int resolveIterations = jointGroup->GetResolveIterations();
            if (ImGui::DragInt("Resolve Iterations", &resolveIterations, 1.0f, 1, 10)) {
                jointGroup->SetResolveIterations(resolveIterations);
            }
        }
        ImGui::PopID();
    }

    // JointComponentのプロパティ表示
    auto* joint = gameObject->GetComponent<JointComponent>();
    if (joint) {
        ImGui::PushID(joint);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Joint", ImGuiTreeNodeFlags_DefaultOpen)) {
            unsigned int connectedObjectID = joint->GetConnectedBodyID();
            if (ImGui::DragScalar("Connected Object ID", ImGuiDataType_U32, &connectedObjectID, 1.0f, nullptr, nullptr)) {
                joint->SetConnectedBodyID(connectedObjectID);
            }

            float restLength = joint->GetRestLength();
            if (ImGui::DragFloat("Rest Length", &restLength, 0.1f, 0.0f, 100.0f)) {
                joint->SetRestLength(restLength);
            }
        }
        ImGui::PopID();
    }

    // ModelComponentのプロパティ表示
    auto* model = gameObject->GetComponent<ModelComponent>();
    if (model) {
        if (BeginComponentSection(model, "Model")){
            // モデルリソースの名前を表示
            std::string modelName = model->GetModelResource()->filePath;
            ImGui::Text("Model Resource:");
            ImGui::Text("  %s", modelName.c_str());
            
            unsigned int materialCount = model->GetMaterialSlots().size();
            ImGui::Text("Material Slots: %d", materialCount);

            for (int i = 0; i < materialCount; i++) {
                auto& materialInstance = model->GetMaterialSlots()[i];
                ImGui::PushID(i);

                // スロット単位でまとめる
                if (ImGui::TreeNode(("Slot " + std::to_string(i)).c_str())) {
                    ImGui::BeginChild("MaterialSlot", ImVec2(0, 300), true);
                    std::string matName = materialInstance.materialResource ? materialInstance.materialResource->name : "None";
                    auto split = MiString::Split(matName, '%');

                    // "Material%Default"のような形式の場合、"Default"のみ表示する
                    matName = split.size() > 2 ? split[2] : matName;
                    
                    ImGui::Text("Material:");
                    ImGui::Text("  %s", matName.c_str());

                    // オーバーライド設定
                    bool overrideBaseColor = materialInstance.isOverrideBaseColor;
                    if (ImGui::Checkbox("Override Base Color", &overrideBaseColor)) {
                        materialInstance.isOverrideBaseColor = overrideBaseColor;
                    }
                    if (materialInstance.isOverrideBaseColor) {
                        auto baseColor = materialInstance.overrideBaseColor;
                        if (ImGui::ColorEdit4("Base Color", &baseColor.x)) {
                            materialInstance.overrideBaseColor = baseColor;
                        }
                    }
                    bool overrideEmissiveColor = materialInstance.isOverrideEmissive;
                    if (ImGui::Checkbox("Override Emissive Color", &overrideEmissiveColor)) {
                        materialInstance.isOverrideEmissive = overrideEmissiveColor;
                    }
                    if (materialInstance.isOverrideEmissive) {
                        XMFLOAT3 emissiveColor = materialInstance.overrideEmissiveColor;
                        if (ImGui::ColorEdit3("Emissive Color", &emissiveColor.x)) {
                            materialInstance.overrideEmissiveColor = emissiveColor;
                        }
                        float emissiveIntensity = materialInstance.overrideEmissiveIntensity;
                        if (ImGui::DragFloat("Emissive Intensity", &emissiveIntensity, 0.1f, 0.0f, 10.0f)) {
                            materialInstance.overrideEmissiveIntensity = emissiveIntensity;
                        }
                    }

                    ImGui::Separator();

                    MaterialResource* matRes = materialInstance.materialResource;
                    if (matRes) {
                        char buffer[256];

                        // マテリアルリソースの名前を表示
                        std::string matResName = matRes->name;
                        ImGui::Text("Material Resource:");
                        ImGui::Text("  %s", matResName.c_str());

                        ImGui::Text("Shader Program:");
                        std::string shaderName = matRes->shaderProgram ? matRes->shaderProgram->name : "None";
                        strcpy(buffer, shaderName.c_str());
                        if (ImGui::InputText("Shader Program", buffer, sizeof(buffer))) {
                            shaderName = buffer;
                            ShaderProgramResource* newShaderRes = SHADER_REPOSITORY->GetShaderProgramResource(shaderName);
                            if (newShaderRes) {
                                matRes->shaderProgram = newShaderRes;
                            }
                        }

                        // マテリアルリソースのテクスチャ
                        ImGui::Text("Textures:");
                        std::string newTexName;
                        TextureResource* newTexRes = nullptr;

                        std::wstring albedoTex = matRes->albedoTexture ? matRes->albedoTexture->name : L"None";
                        strncpy(buffer, (char*)MiString::ToUTF8(albedoTex).c_str(), sizeof(buffer));
                        if (ImGui::InputText("Albedo Texture", buffer, sizeof(buffer))) {
                            newTexName = buffer;
                            albedoTex = MiString::ToWString(newTexName);
                            newTexRes = TEXTURE_REPOSITORY->GetTextureResource(albedoTex);
                            if (newTexRes) {
                                matRes->albedoTexture = newTexRes;
                            }
                        }

                        std::wstring normalTex = matRes->normalTexture ? matRes->normalTexture->name : L"None";
                        strncpy(buffer, (char*)MiString::ToUTF8(normalTex).c_str(), sizeof(buffer));
                        if (ImGui::InputText("Normal Texture", buffer, sizeof(buffer))) {
                            newTexName = buffer;
                            normalTex = MiString::ToWString(newTexName);
                            newTexRes = TEXTURE_REPOSITORY->GetTextureResource(normalTex);
                            if (newTexRes) {
                                matRes->normalTexture = newTexRes;
                            }
                        }

                        std::wstring emissiveTex = matRes->emissiveTexture ? matRes->emissiveTexture->name : L"None";
                        strncpy(buffer, (char*)MiString::ToUTF8(emissiveTex).c_str(), sizeof(buffer));
                        if (ImGui::InputText("Emissive Texture", buffer, sizeof(buffer))) {
                            newTexName = buffer;
                            emissiveTex = MiString::ToWString(newTexName);
                            newTexRes = TEXTURE_REPOSITORY->GetTextureResource(emissiveTex);
                            if (newTexRes) {
                                matRes->emissiveTexture = newTexRes;
                            }
                        }

                        std::wstring aoTex = matRes->aoTexture ? matRes->aoTexture->name : L"None";
                        strncpy(buffer, (char*)MiString::ToUTF8(aoTex).c_str(), sizeof(buffer));
                        if (ImGui::InputText("AO Texture", buffer, sizeof(buffer))) {
                            newTexName = buffer;
                            aoTex = MiString::ToWString(newTexName);
                            newTexRes = TEXTURE_REPOSITORY->GetTextureResource(aoTex);
                            if (newTexRes) {
                                matRes->aoTexture = newTexRes;
                            }
                        }

                        // マテリアルリソースのプロパティ
                        ImGui::Text("Params:");

                        XMFLOAT4 baseColor = matRes->baseColor;
                        if (ImGui::ColorEdit4("Base Color", &baseColor.x)) {
                            matRes->baseColor = baseColor;
                        }
                        XMFLOAT2 uvTiling = matRes->uvTiling;
                        if (ImGui::DragFloat2("UV Tiling", &uvTiling.x, 0.1f)) {
                            matRes->uvTiling = uvTiling;
                        }
                        XMFLOAT2 uvOffset = matRes->uvOffset;
                        if (ImGui::DragFloat2("UV Offset", &uvOffset.x, 0.1f)) {
                            matRes->uvOffset = uvOffset;
                        }
                        float metallic = matRes->metallic;
                        if (ImGui::DragFloat("Metallic", &metallic, 0.01f, 0.0f, 1.0f)) {
                            matRes->metallic = metallic;
                        }
                        float roughness = matRes->roughness;
                        if (ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.0f, 1.0f)) {
                            matRes->roughness = roughness;
                        }
                        XMFLOAT3 emissiveColor = matRes->emissiveColor;
                        if (ImGui::ColorEdit3("Emissive Color", &emissiveColor.x)) {
                            matRes->emissiveColor = emissiveColor;
                        }
                        float emissiveIntensity = matRes->emissiveIntensity;
                        if (ImGui::DragFloat("Emissive Intensity", &emissiveIntensity, 0.1f, 0.0f, 10.0f)) {
                            matRes->emissiveIntensity = emissiveIntensity;
                        }

                    }

                    ImGui::EndChild();
                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
        }

        EndComponentSection();
    }

    // DecalComponentのプロパティ表示
    auto* decal = gameObject->GetComponent<DecalComponent>();
    if (decal) {
        ImGui::PushID(decal);
        ImGui::Separator();

        bool enable = decal->GetEnable();
        if (!enable) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
        }
        bool prevEnable = enable;

        if (ImGui::CollapsingHeader("Decal", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Enable", &enable)) {
                decal->SetEnable(enable);
            }

            auto projSize = decal->GetProjectionSize();
            if (ImGui::DragFloat2("Projection Size", &projSize.x, 0.1f)) {
                decal->SetProjectionSize(projSize);
            }
            auto projDepth = decal->GetProjectionDepth();
            if (ImGui::DragFloat("Projection Depth", &projDepth, 0.1f, 0.0f, 100.0f)) {
                decal->SetProjectionDepth(projDepth);
            }
        }

        if (!prevEnable) {
            ImGui::PopStyleVar();
        }

        ImGui::PopID();
    }

    auto* particleSystem = gameObject->GetComponent<ParticleSystemComponent>();
    if (particleSystem) {
        if (BeginComponentSection(particleSystem, "Particle System")) {
            ImGui::Text("Particles: %zu / %d",
                particleSystem->Particles().size(),
                ParticleSystemComponent::MAX_PARTICLES);

            if (particleSystem->IsPlaying()) {
                if (ImGui::Button("Stop")) {
                    particleSystem->Stop();
                }
            }
            else {
                if (ImGui::Button("Play")) {
                    particleSystem->Play();
                }
            }

            ImGui::Separator();

            auto& main = particleSystem->Main();
            if (ImGui::TreeNode("Main")) {
                ImGui::DragFloat("Duration", &main.duration, 0.01f, 0.0f);
                ImGui::Checkbox("Loop", &main.loop);
                ImGui::Checkbox("Play On Awake", &main.playOnAwake);
                DrawMinMaxFloat("Start Lifetime", main.startLifetime);
                DrawMinMaxFloat("Start Speed", main.startSpeed);
                DrawMinMaxFloat("Start Size", main.startSize);
                DrawMinMaxColor("Start Color", main.startColor);
                ImGui::DragFloat3("Gravity", &main.gravity.x, 0.01f);
                ImGui::DragFloat("Simulation Speed", &main.simulationSpeed, 0.01f, 0.0f);

                int simulationSpace = static_cast<int>(main.simulationSpace);
                const char* simulationSpaceItems[] = { "Local", "World" };
                if (ImGui::Combo("Simulation Space", &simulationSpace, simulationSpaceItems, IM_ARRAYSIZE(simulationSpaceItems))) {
                    main.simulationSpace = static_cast<ParticleSystemComponent::SimulationSpace>(simulationSpace);
                }
                ImGui::TreePop();
            }

            auto& emission = particleSystem->Emission();
            if (ImGui::TreeNode("Emission")) {
                ImGui::Checkbox("Enabled", &emission.enabled);
                ImGui::DragFloat("Rate Over Time", &emission.rateOverTime, 0.1f, 0.0f);
                ImGui::DragFloat("Rate Over Distance", &emission.rateOverDistance, 0.1f, 0.0f);
                ImGui::TreePop();
            }

            auto& shape = particleSystem->Shape();
            if (ImGui::TreeNode("Shape")) {
                ImGui::Checkbox("Enabled", &shape.enabled);
                int shapeType = static_cast<int>(shape.type);
                const char* shapeTypeItems[] = { "Sphere", "Cone" };
                if (ImGui::Combo("Shape Type", &shapeType, shapeTypeItems, IM_ARRAYSIZE(shapeTypeItems))) {
                    shape.type = static_cast<ParticleSystemComponent::ShapeType>(shapeType);
                }

                if (shape.type == ParticleSystemComponent::ShapeType::Sphere) {
                    ImGui::DragFloat("Sphere Radius", &shape.sphere.radius, 0.01f, 0.0f);
                    ImGui::Checkbox("Emit From Shell", &shape.sphere.emitFromShell);
                }
                else if (shape.type == ParticleSystemComponent::ShapeType::Cone) {
                    float coneAngleDegrees = XMConvertToDegrees(shape.cone.angle);
                    if (ImGui::DragFloat("Cone Angle", &coneAngleDegrees, 0.1f, 0.0f, 180.0f)) {
                        shape.cone.angle = XMConvertToRadians(coneAngleDegrees);
                    }
                    ImGui::DragFloat("Cone Radius", &shape.cone.radius, 0.01f, 0.0f);
                    ImGui::DragFloat("Cone Length", &shape.cone.length, 0.01f, 0.0f);
                    ImGui::Checkbox("Emit From Base", &shape.cone.emitFromBase);
                }

                ImGui::DragFloat("Random Direction Amount", &shape.randomDirectionAmount, 0.01f, 0.0f, 1.0f);
                ImGui::TreePop();
            }

            auto& sizeOverLifetime = particleSystem->SizeOverLifetime();
            if (ImGui::TreeNode("Size Over Lifetime")) {
                ImGui::Checkbox("Enabled", &sizeOverLifetime.enabled);
                DrawFloatCurve("Size Curve", sizeOverLifetime.size);
                ImGui::TreePop();
            }

            auto& renderer = particleSystem->Renderer();
            if (ImGui::TreeNode("Renderer")) {
                if (renderer.textureResource) {
                    ImGui::Text("Texture: %s", MiString::ToUTF8(renderer.textureResource->name).c_str());
                }
                else {
                    ImGui::Text("Texture: None");
                }

                ImGui::DragFloat4("UV Rect", &renderer.uvRect.x, 0.01f);

                int billboardMode = static_cast<int>(renderer.billboardMode);
                const char* billboardModeItems[] = { "View", "Horizontal" };
                if (ImGui::Combo("Billboard Mode", &billboardMode, billboardModeItems, IM_ARRAYSIZE(billboardModeItems))) {
                    renderer.billboardMode = static_cast<ParticleSystemComponent::BillboardMode>(billboardMode);
                }

                int blendMode = static_cast<int>(renderer.blendMode);
                const char* blendModeItems[] = { "Alpha Blend", "Additive" };
                if (ImGui::Combo("Blend Mode", &blendMode, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
                    renderer.blendMode = static_cast<ParticleSystemComponent::BlendMode>(blendMode);
                }

                ImGui::Checkbox("Sort By Distance", &renderer.sortByDistance);
                ImGui::TreePop();
            }
        }

        EndComponentSection();
    }

#pragma region UIComponents
    auto* text = gameObject->GetComponent<TextComponent>();
    if (text) {
        ImGui::PushID(text);
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Text", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto textString = text->GetText();
            char buffer[256];
            strncpy(buffer, (char*)textString.c_str(), sizeof(buffer));
            if (ImGui::InputText("Text String", buffer, sizeof(buffer))) {
                text->SetText(buffer);
            }

            auto color = text->GetColor();
            if (ImGui::ColorEdit4("Color", &color.x)) {
                text->SetColor(color);
            }
            int fontSize = text->GetFontSize();
            if (ImGui::DragInt("Font Size", &fontSize, 1.0f, 1, 512)) {
                text->SetFontSize(fontSize);
            }
            bool center = text->IsCenter();
            if (ImGui::Checkbox("Center", &center)) {
                text->SetCenter(center);
            }
        }
        ImGui::PopID();
    }
    auto* image = gameObject->GetComponent<ImageComponent>();
    if (image) {
        ImGui::PushID(image);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto color = image->GetColor();
            if (ImGui::ColorEdit4("Color", &color.x)) {
                image->SetColor(color);
            }
        }
        ImGui::PopID();
    }
    auto* slider = gameObject->GetComponent<SliderComponent>();
    if (slider) {
        ImGui::PushID(slider);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Slider", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto value = slider->GetValue();
            if (ImGui::DragFloat("Value", &value, 0.01f, 0.0f, 1.0f)) {
                slider->SetValue(value);
            }
            auto color = slider->GetBgColor();
            if (ImGui::ColorEdit4("Bg Color", &color.x)) {
                slider->SetBgColor(color);
            }
            auto fillColor = slider->GetFillColor();
            if (ImGui::ColorEdit4("Fill Color", &fillColor.x)) {
                slider->SetFillColor(fillColor);
            }
        }
        ImGui::PopID();
    }

#pragma endregion
    

}

bool InspectorViewWindow::BeginComponentSection(Component* comp,const char* name, bool useEnableSetting)
{
    ImGui::PushID(comp);
    ImGui::Separator();

    bool open = ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen);
    bool enable = true;
    if (open && useEnableSetting) {
        enable = comp->GetEnable();
        if (ImGui::Checkbox("Enable", &enable)) {
            comp->SetEnable(enable);
        }
    }

    if (enable) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
    }
    else {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
    }

    return open;
}

void InspectorViewWindow::EndComponentSection()
{
    ImGui::PopStyleVar();
    ImGui::PopID();
}
