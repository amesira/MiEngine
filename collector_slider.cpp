//===================================================
// collector_slider.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/11/27
//===================================================
#include "collector_slider.h"
#include "scene_interface.h"
#include "game_object.h"

#include "direct3d.h"

#include "rect_transform_component.h"
#include "slider_component.h"
#include "transform_component.h"

#include "engine_service_locator.h"

static ID3D11ShaderResourceView* g_pTexture = nullptr;

void CollectorSlider::Initialize()
{
    //LoadTexture(&g_pTexture, L"asset\\Texture\\white.bmp");
    auto resource = EngineServiceLocator::GetTextureRepository()->GetTextureResource(L"asset\\Texture\\white.bmp");
    if (resource) {
        g_pTexture = resource->texture.Get();
    }
}

void CollectorSlider::Finalize()
{

}

void CollectorSlider::CollectDrawBatches2D(IScene* pScene, std::vector<DrawBatch2D>& outBatches) 
{
    auto* sliderComponentPool = pScene->GetComponentPool<SliderComponent>();
    auto* rectTransformPool = pScene->GetComponentPool<RectTransformComponent>();
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();
    if (sliderComponentPool == nullptr)return;

    auto& sliderList = sliderComponentPool->GetList();

    for (SliderComponent& sliderComp : sliderList)
    {
        SliderComponent* slider = &sliderComp;
        RectTransformComponent* rect = rectTransformPool->GetByGameObjectID(slider->GetOwner()->GetID());

        if (!rect) continue;
        if (!slider->GetOwner()->GetActive()) continue;
        if (!slider->GetEnable()) continue;

        // 描画コマンドに追加
        DrawBatch2D batch;
        batch.orderInLayer = rect->GetPosition().z;
        batch.texture = g_pTexture;

        // BG描画コマンド
        DrawCommand2DInstance instance;
        instance.position = { rect->GetPosition().x, rect->GetPosition().y };
        instance.size = { rect->GetScaling().x, rect->GetScaling().y };
        instance.angleZ = rect->GetRotation().z;
        instance.color = slider->GetBgColor();
        instance.uvRect = {0.0f, 0.0f, 1.0f, 1.0f};

        batch.instances.push_back(instance);

        // Magicalでは、ここに遅延塗りつぶしを描画している

        // 塗りつぶし描画コマンド
        float value = slider->GetValue();
        {
            XMFLOAT2 fillLocalPos = { -(rect->GetScaling().x * (1.0f - value)) * 0.5f, 0.0f };
            float cosA = cosf(instance.angleZ);
            float sinA = sinf(instance.angleZ);
            XMFLOAT2 fillRotatedPos = {
                fillLocalPos.x * cosA - fillLocalPos.y * sinA,
                fillLocalPos.x * sinA + fillLocalPos.y * cosA
            };
            instance.position = {
                rect->GetPosition().x + fillRotatedPos.x,
                rect->GetPosition().y + fillRotatedPos.y
            };
        }
        instance.size.x = rect->GetScaling().x * value;
        instance.color = slider->GetFillColor();

        batch.instances.push_back(instance);

        outBatches.push_back(batch);
    }
}