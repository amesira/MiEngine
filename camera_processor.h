//---------------------------------------------------
// camera_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/29
//---------------------------------------------------
#ifndef CAMERA_PROCESSOR_H
#define CAMERA_PROCESSOR_H
#include "processor.h"
#include <string>
#include "direct3d.h"

#include "render_view.h"

#include <DirectXMath.h>
using namespace DirectX;

class CameraComponent;

class CameraProcessor : public Processor {
private:
    static const int MAX_CAMERAS = 8;
    int m_cameraCounter = 0;
    CameraComponent* m_cameras[MAX_CAMERAS] = { nullptr };

public:
    void    Initialize() override;
    void    Finalize() override;
    void    Process(IScene* pScene) override;

    // RenderViewの取得
    std::vector<RenderView> GetRenderViews() const ;

};
#endif // CAMERA_PROCESSOR_H