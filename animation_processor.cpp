//===================================================
// animation_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/06
//===================================================
#include "animation_processor.h"
#include "scene_interface.h"
#include "game_object.h"

#include "model_component.h"

void AnimationProcessor::Initialize() 
{
}

void AnimationProcessor::Finalize() 
{
}

void AnimationProcessor::Process(IScene* pScene) 
{
    // コンポーネントプール取得
    auto* modelPool = pScene->GetComponentPool<ModelComponent>();
    if (!modelPool)return;

    // アニメーションの更新

}