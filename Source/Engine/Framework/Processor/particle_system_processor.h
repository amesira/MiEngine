//---------------------------------------------------
// particle_system_processor.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//---------------------------------------------------
#ifndef PARTICLE_SYSTEM_PROCESSOR_H
#define PARTICLE_SYSTEM_PROCESSOR_H
#include "Engine/Core/processor.h"
#include "Engine/Device/direct3d.h"
using namespace DirectX;

class ParticleSystemProcessor : public Processor {
public:
    void Initialize() override;
    void Finalize() override;
    void Process(IScene* pScene) override;
};

#endif // PARTICLE_SYSTEM_PROCESSOR_H
