//===================================================
// collector_image.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
#include "collector_image.h"
#include "scene_interface.h"
#include "game_object.h"

#include "rect_transform_component.h"
#include "image_component.h"

// 初期化
void CollectorImage::Initialize()
{

}

// 終了処理
void CollectorImage::Finalize()
{

}

// 2D描画用のDrawBatch2Dコマンドを収集
void CollectorImage::CollectDrawBatches2D(IScene* pScene, std::vector<DrawBatch2D>& outBatches)
{
    auto* imagePool = pScene->GetComponentPool<ImageComponent>();
    auto* rectTransformPool = pScene->GetComponentPool<RectTransformComponent>();
    if(imagePool == nullptr || rectTransformPool == nullptr)return;

    auto& imageList = imagePool->GetList();

    for (auto& imageComp : imageList) {
        auto* image = &imageComp;
        auto* rect = rectTransformPool->GetByGameObjectID(image->GetOwner()->GetID());

        if (!rect) continue;
        if (!image->GetEnable()) continue;
        if (!image->GetOwner()->GetActive()) continue;

        // 描画コマンドに追加
        DrawBatch2D batch;
        batch.orderInLayer = rect->GetPosition().z;
        batch.texture = image->GetTexture();

        DrawCommand2DInstance instance;
        instance.position = { rect->GetPosition().x, rect->GetPosition().y };
        instance.size = { rect->GetScaling().x, rect->GetScaling().y };
        instance.angleZ = rect->GetRotation().z;
        instance.color = image->GetColor();
        instance.uvRect = image->GetUvRect();

        batch.instances.push_back(instance);

        outBatches.push_back(batch);
    }
}
