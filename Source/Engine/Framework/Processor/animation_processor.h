//---------------------------------------------------
// animation_processor.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/06
//---------------------------------------------------
#ifndef ANIMATION_PROCESSOR_H
#define ANIMATION_PROCESSOR_H
#include "Engine/Core/processor.h"
#include "Engine/System/Graphics/model_resource.h"

class AnimationProcessor : public Processor {
private:

public:
    void Initialize() override;
    void Finalize() override;
    void Process(IScene* pScene) override;

private:
    // キーフレームの線形補間
    XMFLOAT4 SamplingKeyframes(const std::vector<AnimationClip::Keyframe>& keyframes, float time);

};

#endif // ANIMATION_PROCESSOR_H