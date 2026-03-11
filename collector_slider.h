//----------------------------------------------------
// collector_slider.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/27
//----------------------------------------------------
#ifndef COLLECTOR_SLIDER_H
#define COLLECTOR_SLIDER_H
#include "draw_command.h"
class IScene;

class CollectorSlider  {
public:
    void Initialize();
    void Finalize();

    void CollectDrawBatches2D(IScene* pScene, std::vector<DrawBatch2D>& outBatches);
};

#endif