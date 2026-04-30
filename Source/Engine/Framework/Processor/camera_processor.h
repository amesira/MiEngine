//---------------------------------------------------
// camera_processor.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/29
//---------------------------------------------------
#ifndef CAMERA_PROCESSOR_H
#define CAMERA_PROCESSOR_H
#include "Engine/Core/processor.h"
#include "Engine/System/Device/direct3d.h"
using namespace DirectX;

#include <string>
#include <vector>

#include "Engine/render_view.h"

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
    void SetRenderViews(std::vector<RenderView>& outViews);

    // CameraCounterの取得
    int GetCameraCounter() const { return m_cameraCounter; }

};
#endif // CAMERA_PROCESSOR_H