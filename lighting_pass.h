//---------------------------------------------------
// lighting_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/05
//---------------------------------------------------
#ifndef LIGHTING_PASS_H
#define LIGHTING_PASS_H
#include "pass.h"

class LightingPass : public Pass {
private:
    int m_lightCount;

public:
    void Initialize() override;
    void Finalize() override;
    void Process(IScene* pScene) override;

    void SetLightEnable(bool enable);

};

#endif // LIGHT_PROCESSOR_H