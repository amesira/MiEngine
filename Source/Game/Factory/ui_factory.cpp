//===================================================
// ui_factory.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//===================================================
#include "ui_factory.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_base.h"

// component
#include "Engine/Framework/Component/rect_transform_component.h"
#include "Engine/Framework/Component/image_component.h"
#include "Engine/Framework/Component/text_component.h"
#include "Engine/Framework/Component/slider_component.h"

#include "Engine/engine_service_locator.h"
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

// UIイメージ生成
GameObject* UiFactory::CreateUiImage(SceneBase* scene, const std::wstring& texturePath)
{
    GameObject* uiImage = scene->CreateGameObject();
    uiImage->SetName("UiImage");

    // component生成・登録
    RectTransformComponent* rectTransform = uiImage->AddComponent<RectTransformComponent>();
    ImageComponent* imageComp = uiImage->AddComponent<ImageComponent>();

    TextureResource* textureResource = TEXTURE_REPOSITORY->GetTextureResource(texturePath);
    imageComp->SetTextureResource(textureResource);

    return uiImage;
}

// UIテキスト生成
GameObject* UiFactory::CreateUiText(SceneBase* scene, const std::u8string& text)
{
    GameObject* uiText = scene->CreateGameObject();
    uiText->SetName("UiText");

    // component生成・登録
    RectTransformComponent* rectTransform = uiText->AddComponent<RectTransformComponent>();
    TextComponent* textComp = uiText->AddComponent<TextComponent>();
    textComp->SetText(text);
    textComp->SetFontSize(32.0f);
    textComp->SetColor({ 1.0f, 0.5f, 0.5f, 1.0f });
    textComp->SetCenter(true);

    return uiText;
}

// UIスライダー生成
GameObject* UiFactory::CreateUiSlider(SceneBase* scene, const XMFLOAT4& bgColor, const XMFLOAT4& fillColor, float value)
{
    GameObject* uiSlider = scene->CreateGameObject();
    uiSlider->SetName("UiSlider");

    // component生成・登録
    RectTransformComponent* rectTransform = uiSlider->AddComponent<RectTransformComponent>();
    SliderComponent* sliderComp = uiSlider->AddComponent<SliderComponent>();

    sliderComp->SetBgColor(bgColor);
    sliderComp->SetFillColor(fillColor);
    sliderComp->SetValue(value);

    return uiSlider;
}

// UITransformセットアップ
bool UiFactory::SetupUiTransform(GameObject* uiElement, const XMFLOAT2& position, const XMFLOAT2& size)
{
    if (!uiElement) return false;
    RectTransformComponent* rectTransform = uiElement->GetComponent<RectTransformComponent>();
    if (!rectTransform) return false;

    rectTransform->SetPosition({ position.x, position.y, 0.0f });
    rectTransform->SetScaling({ size.x, size.y, 1.0f });

    return true;
}