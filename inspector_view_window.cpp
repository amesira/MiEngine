//===================================================
// inspector_view_window.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/12
//===================================================
#include "inspector_view_window.h"
#include "engine.h"

#include "editor_context.h"
#include "render_view.h"
#include "game_object.h"

#include "mi_string.h"

#include "behavior_component.h"

// 主要なComponentのヘッダをインクルード
#include "transform_component.h"
#include "rect_transform_component.h"

#include "camera_component.h"
#include "light_component.h"

#include "rigidbody_component.h"
#include "collider_component.h"

#include "text_component.h"
#include "image_component.h"
#include "slider_component.h"

#include "model_component.h"

#include "joint_group_component.h"
#include "joint_component.h"


void InspectorViewWindow::Draw()
{
    ImGui::SetNextWindowPos({ m_editorContext->displayX - m_editorContext->inspectorWidth , m_editorContext->toolbarHeight });
    float width = m_editorContext->inspectorWidth;
    float height = m_editorContext->displayY - m_editorContext->toolbarHeight;
    ImGui::SetNextWindowSize({ width, height });

    ImGui::Begin("Inspector View", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse);

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
            ImGui::BeginChild("Component Inspector", ImVec2(0, 400), true);
            DrawComponentInspector(m_editorContext->selectedObject);
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }

        ImGui::Separator();

        // BehaviorComponentのプロパティ表示
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.23f, 0.18f, 1.0f));
            ImGui::BeginChild("Behavior Component Inspector", ImVec2(0, 0), true);
            for (auto* behavior : behaviorComponents) {
                // ここでBehaviorComponent固有のプロパティを表示するコードを追加
                behavior->DrawComponentInspector();
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
    }

    ImGui::End();
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
            auto eulerRotation = transform->GetEulerRotation();
            if (ImGui::DragFloat3("Rotation", &eulerRotation.x, 0.1f)) {
                transform->SetEulerRotation(eulerRotation);
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
        ImGui::PushID(rigidbody);
        ImGui::Separator();

        // Rigidbodyが無効な場合は半透明にする
        bool enable = rigidbody->GetEnable();
        if (!enable) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
        }
        bool prevEnable = enable;

        if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Enable", &enable)) {
                rigidbody->SetEnable(enable);
            }

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

        if (!prevEnable) {
            ImGui::PopStyleVar();
        }

        ImGui::PopID();
    }

    // ColliderComponentのプロパティ表示
    auto* boxCollider = gameObject->GetComponent<BoxColliderComponent>();
    if (boxCollider) {
        ImGui::PushID(boxCollider);
        ImGui::Separator();

        // enable
        bool enable = boxCollider->GetEnable();
        if (!enable) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
        }
        bool prevEnable = enable;

        if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Enable", &enable)) {
                boxCollider->SetEnable(enable);
            }

            auto center = boxCollider->GetCenter();
            if (ImGui::DragFloat3("Center", &center.x, 0.1f)) {
                boxCollider->SetCenter(center);
            }
            auto scale = boxCollider->GetScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
                boxCollider->SetScale(scale);
            }
        }

        if (!prevEnable) {
            ImGui::PopStyleVar();
        }

        ImGui::PopID();
    }
    auto* sphereCollider = gameObject->GetComponent<SphereColliderComponent>();
    if (sphereCollider) {
        ImGui::PushID(sphereCollider);
        ImGui::Separator();

        bool enable = sphereCollider->GetEnable();
        if (!enable) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
        }
        bool prevEnable = enable;

        if (ImGui::CollapsingHeader("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Enable", &enable)) {
                sphereCollider->SetEnable(enable);
            }

            auto center = sphereCollider->GetCenter();
            if (ImGui::DragFloat3("Center", &center.x, 0.1f)) {
                sphereCollider->SetCenter(center);
            }
            float radius = sphereCollider->GetRadius();
            if (ImGui::DragFloat("Radius", &radius, 0.1f)) {
                sphereCollider->SetRadius(radius);
            }
        }

        if (!prevEnable) {
            ImGui::PopStyleVar();
        }

        ImGui::PopID();
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
        ImGui::PushID(model);
        ImGui::Separator();

        bool enable = model->GetEnable();
        if (!enable) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
        }
        bool prevEnable = enable;

        if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Enable", &enable)) {
                model->SetEnable(enable);
            }

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
                    bool overrideEmissiveColor = materialInstance.isOverrideEmissiveColor;
                    if (ImGui::Checkbox("Override Emissive Color", &overrideEmissiveColor)) {
                        materialInstance.isOverrideEmissiveColor = overrideEmissiveColor;
                    }
                    if (materialInstance.isOverrideEmissiveColor) {
                        auto emissiveColor = materialInstance.overrideEmissiveColor;
                        if (ImGui::ColorEdit3("Emissive Color", &emissiveColor.x)) {
                            materialInstance.overrideEmissiveColor = emissiveColor;
                        }
                    }

                    ImGui::Separator();

                    // マテリアルリソースのプロパティも表示する
                    float metallic = materialInstance.materialResource ? materialInstance.materialResource->metallic : 0.0f;
                    if (ImGui::DragFloat("Metallic", &metallic, 0.01f, 0.0f, 1.0f)) {
                        if (materialInstance.materialResource) {
                            materialInstance.materialResource->metallic = metallic;
                        }
                    }
                    float roughness = materialInstance.materialResource ? materialInstance.materialResource->roughness : 1.0f;
                    if (ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.0f, 1.0f)) {
                        if (materialInstance.materialResource) {
                            materialInstance.materialResource->roughness = roughness;
                        }
                    }


                    ImGui::EndChild();
                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
        }

        if (!prevEnable) {
            ImGui::PopStyleVar();
        }

        ImGui::PopID();
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
            if (ImGui::DragInt("Font Size", &fontSize, 1.0f, 1, 256)) {
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
