//===================================================
// camera_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/12/29
//===================================================
#include "camera_processor.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/System/Device/mi_fps.h"
#include "Utility/mi_math.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/camera_component.h"

void CameraProcessor::Initialize()
{
    
}

void CameraProcessor::Finalize()
{

}

void CameraProcessor::Process(IScene* pScene)
{
    auto* transformCompPool = pScene->GetComponentPool<TransformComponent>();
    auto* cameraCompPool = pScene->GetComponentPool<CameraComponent>();

    if(!transformCompPool || !cameraCompPool)return;

    auto& cameraCompList = cameraCompPool->GetList();

    m_cameraCounter = 0;
    for (CameraComponent& camera : cameraCompList) {
        CameraComponent* c = &camera;
        TransformComponent* transform = transformCompPool->GetByGameObjectID(camera.GetOwner()->GetID());
        
        // 無効なコンポーネントはスキップ
        if (!transform || !c) continue;
        if (!transform->GetEnable() || !c->GetEnable()) continue;
        if (camera.GetOwner()->GetActive() == false) continue;

        // gameObjectIDsに登録
        if (m_cameraCounter >= MAX_CAMERAS)break;

        // ビュー行列、プロジェクション行列計算
        XMVECTOR vPos = XMVectorSet(
            transform->GetPosition().x,
            transform->GetPosition().y,
            transform->GetPosition().z,
            0.0f);
        XMVECTOR vAt = XMVectorSet(
            camera.GetAtPosition().x,
            camera.GetAtPosition().y,
            camera.GetAtPosition().z,
            0.0f);
        XMVECTOR vUp = XMVectorSet(
            camera.GetUpVector().x,
            camera.GetUpVector().y,
            camera.GetUpVector().z,
            0.0f);

        XMMATRIX view = XMMatrixLookAtLH(vPos, vAt, vUp);
        XMMATRIX projection = XMMatrixPerspectiveFovLH(
            XMConvertToRadians(camera.GetFov()),
            camera.GetAspect(),
            camera.GetNearClip(),
            camera.GetFarClip());

        camera.SetViewMatrix(view);
        camera.SetProjectionMatrix(projection);
        camera.SetEyePosition(transform->GetPosition());

        m_cameras[m_cameraCounter] = c;

        m_cameraCounter++;
    }
}

// 描画に必要な情報をまとめる構造体RenderViewのリストを取得
void CameraProcessor::SetRenderViews(std::vector<RenderView>& outViews)
{
    for (int i = 0; i < m_cameraCounter; i++) {
        CameraComponent* camera = m_cameras[i];
        if (!camera) continue;

        for (int j = 0; j < outViews.size(); j++) {
            if (outViews[j].enabled) continue; // すでに有効なRenderViewがある場合はスキップ
            // RenderViewにカメラ情報を設定
            RenderView& view = outViews[i];
            view.enabled = true;

            view.viewMatrix = camera->GetViewMatrix();
            view.projectionMatrix = camera->GetProjectionMatrix();
            view.eyePosition = camera->GetEyePosition();
            view.aspectRatio = camera->GetAspect();

            view.enable3D = true;
            view.enableLighting = true;
            view.enableUI = true;
            view.enablePostEffect = true;
            view.enableShadowMap = true;
            view.enableDebugDraw = false;
            break;
        }
    }
}
