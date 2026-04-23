//===================================================
// render_effect_factory.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//===================================================
#include "render_effect_factory.h"
#include "Engine/Core/game_object.h"
#include "Engine/Core/scene_base.h"

// component
#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/decal_component.h"

#include "Engine/engine_service_locator.h"
#define TEXTURE_REPOSITORY EngineServiceLocator::GetTextureRepository()

// デカールエフェクト生成
GameObject* RenderEffectFactory::CreateDecalEffect(SceneBase* scene, const XMFLOAT3& position, const std::wstring& decalTexturePath)
{
    GameObject* decalEffect = scene->CreateGameObject();
    decalEffect->SetName("DecalEffect");

    // component生成・登録
    TransformComponent* transform = decalEffect->AddComponent<TransformComponent>();
    DecalComponent* decal = decalEffect->AddComponent<DecalComponent>();

    // component設定
    transform->SetPosition(position);
    TextureResource* decalTexture = TEXTURE_REPOSITORY->GetTextureResource(decalTexturePath);
    decal->SetDecalTexture(decalTexture);

    return decalEffect;
}
