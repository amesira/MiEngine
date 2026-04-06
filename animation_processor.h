//---------------------------------------------------
// animation_processor.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/06
//---------------------------------------------------
#ifndef ANIMATION_PROCESSOR_H
#define ANIMATION_PROCESSOR_H
#include "processor.h"

class AnimationProcessor : public Processor {
private:

public:
    void Initialize() override;
    void Finalize() override;
    void Process(IScene* pScene) override;

};

#endif // ANIMATION_PROCESSOR_H