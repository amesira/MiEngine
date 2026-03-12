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

        // enable
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
