//----------------------------------------------------
// collector_image.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//----------------------------------------------------
#ifndef COLLECTOR_IMAGE_H
#define COLLECTOR_IMAGE_H
#include <vector>
#include "draw_command.h"
class IScene;
class ShaderProgramResource;

class CollectorImage {
private:
    ShaderProgramResource* m_pDefaultSpriteShader = nullptr;
    
public:
    void Initialize();
    void Finalize();

    void CollectDrawBatches2D(IScene* pScene, std::vector<DrawBatch2D>& outBatches);

};

#endif