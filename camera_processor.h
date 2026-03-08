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

#include <DirectXMath.h>
using namespace DirectX;

class CameraComponent;

class CameraProcessor : public Processor {
public:
    void    Initialize() override;
    void    Finalize() override;
    void    Process(IScene* pScene) override;

private:
    static const int MAX_CAMERAS = 8;
    int     m_cameraCounter = 0;
    CameraComponent*            m_cameraObjects[MAX_CAMERAS];
    ID3D11ShaderResourceView*   m_cameraSnapshots[MAX_CAMERAS];

public:
    int     GetCameraCount() const { return m_cameraCounter; }

    bool    BindMatrix(int index);
    bool    SnapShotCamera(int index);

    bool    DrawSnapshot(int index, float x, float y, float width, float height, bool usePixelOption = true);

    XMFLOAT4   GetClearColor(int index) const;
};
#endif // CAMERA_PROCESSOR_H