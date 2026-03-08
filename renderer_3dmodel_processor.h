//---------------------------------------------------
// renderer_3dmodel_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/27
//---------------------------------------------------
#ifndef RENDERER_3DMODEL_PROCESSOR_H
#define RENDERER_3DMODEL_PROCESSOR_H

#include "processor.h"
class IScene;

class Renderer3DModelProcessor : public Processor {
public:
    void Initialize() override;
    void Finalize() override;
    void Process(IScene* pScene) override;
};

#endif // RENDERER_3DMODEL_PROCESSOR_H