//----------------------------------------------------
// renderer_slider_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/27
//----------------------------------------------------
#ifndef RENDERER_SLIDER_PROCESSOR_H
#define RENDERER_SLIDER_PROCESSOR_H

#include "processor.h"


class RendererSliderProcessor : public Processor {
public:
    void    Initialize()override;
    void    Finalize()override;
    void    Process(IScene* pScene)override;
};

#endif