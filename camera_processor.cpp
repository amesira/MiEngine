//===================================================
// camera_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/12/29
//===================================================
#include "camera_processor.h"
#include "scene_interface.h"
#include "game_object.h"

#include "transform_component.h"
#include "camera_component.h"

#include "shader.h"
#include "sprite.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pDeviceContext = nullptr;

void CameraProcessor::Initialize()
{
    g_pDevice = Direct3D_GetDevice();
    g_pDeviceContext = Direct3D_GetDeviceContext();

    for(int i = 0; i < 8; ++i) {
        m_cameraObjects[i] = nullptr;
    }
}

void CameraProcessor::Finalize()
{

}

void CameraProcessor::Process(IScene* pScene)
{
    m_cameraCounter = 0;
    for (int i = 0; i < MAX_CAMERAS; ++i) {
        m_cameraObjects[i] = nullptr;
    }

    // コンポーネントプール取得
    auto* transformCompPool = pScene->GetComponentPool<TransformComponent>();
    auto* cameraCompPool = pScene->GetComponentPool<CameraComponent>();

    if(!transformCompPool || !cameraCompPool)return;

    auto& cameraCompList = cameraCompPool->GetList();
    for (CameraComponent& camera : cameraCompList) {
        CameraComponent* c = &camera;
        TransformComponent* transform = camera.GetOwner()->GetComponent<TransformComponent>();
        
        // 無効なコンポーネントはスキップ
        if (!transform && !c) continue;
        if (!transform->GetEnable() || !c->GetEnable()) continue;
        if (camera.GetOwner()->GetActive() == false) continue;

        // gameObjectIDsに登録
        if (m_cameraCounter < MAX_CAMERAS) {
            m_cameraObjects[m_cameraCounter] = &camera;
            m_cameraCounter++;
        } else {
            break; // 最大8つまで
        }

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
    }

    // 優先順などの処理はここに追加可能
    // m_cameraObjectIDsをバブルソートするイメージ

}

bool CameraProcessor::BindMatrix(int index)
{
    // コンポーネント取得
    CameraComponent* camera = m_cameraObjects[index];
    if (!camera) return false;

    // Direct3Dにセット
    Direct3D_SetProjectionMatrix(camera->GetProjectionMatrix());
    Direct3D_SetViewMatrix(camera->GetViewMatrix());

    // カメラ設定
    Direct3D_SetCameraInfo(
        camera->GetOwner()->GetComponent<TransformComponent>()->GetPosition(),
        camera->GetAtPosition()
    );

    return true;
}

bool CameraProcessor::SnapShotCamera(int index)
{
    // コンポーネント取得
    CameraComponent* camera = m_cameraObjects[index];
    if (!camera) return false;

    // スナップショット用テクスチャSRV作成
    Direct3D_CreateSnapshotSceneSRV(&m_cameraSnapshots[index]);
    camera->SetSnapshot(m_cameraSnapshots[index]);

    return true;
}

bool CameraProcessor::DrawSnapshot(int index, float x, float y, float width, float height, bool usePixelOption)
{
    // コンポーネント取得
    CameraComponent* camera = m_cameraObjects[index];
    if (!camera) return false;
    if (!camera->GetSnapshot()) return false;

    // シェーダー設定
    Shader_Begin();
    
    SetBlendState(BLENDSTATE_NONE);

    // スナップショット描画
    ID3D11ShaderResourceView* srv = camera->GetSnapshot();
    g_pDeviceContext->PSSetShaderResources(0, 1, &srv);

    if(usePixelOption){
        // シェーダー
        Shader_SetPixelOption(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), camera->GetShaderGrayRate());
     }

    // スプライト描画
    XMMATRIX scaleMatrix = XMMatrixScaling(width, height, 1.0f);
    XMMATRIX transMatrix = XMMatrixTranslation(x, y, 0.0f);
    XMMATRIX world = scaleMatrix * transMatrix;

    const float screenWidth = (float)Direct3D_GetBackBufferWidth();
    const float screenHeight = (float)Direct3D_GetBackBufferHeight();
    XMMATRIX vp = XMMatrixOrthographicOffCenterLH(
        0.0f,
        screenWidth,
        screenHeight,
        0.0f,
        0.0f,
        1.0f);

    Shader_SetMatrix(world * vp);
    Shader_SetWorldMatrix(world);
    DrawSprite(XMFLOAT4(1.0f,1.0f,1.0f,1.0f),XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

    // リセット
    Shader_SetPixelOption(XMFLOAT4(1.0f,1.0f,1.0f,1.0f), 0.0f);

    return true;
}

XMFLOAT4 CameraProcessor::GetClearColor(int index) const
{
    CameraComponent* camera = m_cameraObjects[index];
    if (!camera) return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    return camera->GetClearColor();
}
