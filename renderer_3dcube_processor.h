//===================================================
// renderer_3dcube_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef RENDERER_3DCUBE_PROCESSOR_H
#define RENDERER_3DCUBE_PROCESSOR_H

#include <vector>
#include "processor.h"

class IScene;

class TransformComponent;
class CubemeshComponent;

class Renderer3DCubeProcessor : public Processor{
private:

public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene);
};

#endif