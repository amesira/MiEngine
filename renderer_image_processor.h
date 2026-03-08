//----------------------------------------------------
// renderer_image_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//----------------------------------------------------
#ifndef RENDERER_IMAGE_PROCESSOR_H
#define RENDERER_IMAGE_PROCESSOR_H

#include <vector>
#include "processor.h"

class RectTransformComponent;
class ImageComponent;

class RendererImageProcessor : public Processor {
private:
    bool    m_drawUiImages = true;
    bool    m_drawWorldImages = true;

public:
    void    Initialize()override;
    void    Finalize()override;
    void    Process(IScene* pScene)override;

    void    SetDrawUiImages(bool draw) { m_drawUiImages = draw; }
    void    SetDrawWorldImages(bool draw) { m_drawWorldImages = draw; }
};

#endif