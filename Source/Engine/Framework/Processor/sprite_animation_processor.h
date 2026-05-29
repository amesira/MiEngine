//---------------------------------------------------
// sprite_animation_processor.h
// 
// Author：Miu Kitamura
// Date  ：2026/05/29
//---------------------------------------------------
#ifndef SPRITE_ANIMATION_PROCESSOR_H
#define SPRITE_ANIMATION_PROCESSOR_H
#include "Engine/Core/processor.h"

class SpriteAnimationProcessor : public Processor {
public:
    void Initialize() override;
    void Finalize() override;
    void Process(IScene* pScene) override;

private:

};

#endif // SPRITE_ANIMATION_PROCESSOR_H