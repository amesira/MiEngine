//---------------------------------------------------
// light_processor.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/05
//---------------------------------------------------
#ifndef LIGHT_PROCESSOR_H
#define LIGHT_PROCESSOR_H

#include "processor.h"

class LightProcessor : public Processor {
public:
    void Initialize() override;
    void Finalize() override;
    void Process(IScene* pScene) override;

public:
    void SetUiLight();
};

#endif // LIGHT_PROCESSOR_H