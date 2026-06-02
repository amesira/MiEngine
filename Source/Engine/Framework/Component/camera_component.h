//---------------------------------------------------
// camera_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/29
//---------------------------------------------------
#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H
#include "Engine/Core/component.h"
#include "Engine/Core/game_object_layer.h"

#include <DirectXMath.h>
using namespace DirectX;

class CameraProcessor;

class CameraComponent : public Component {
private:
    XMFLOAT3    m_atPosition = { 0.0f, 0.0f, 0.0f };// 注視点
    XMFLOAT3    m_upVector = { 0.0f, 1.0f, 0.0f };  // 上方ベクトル

    float       m_fov = 60.0f;              // 視野角（画角）
    float       m_aspect = 16.0f / 9.0f;    // 画面のアスペクト比
    float       m_nearClip = 0.1f;          // 近面クリップ距離
    float       m_farClip = 100.0f;         // 遠面クリップ距離

    XMMATRIX    m_view;         // ビュー行列
    XMMATRIX    m_projection;   // プロジェクション行列
    XMFLOAT3    m_eyePosition = { 0.0f, 0.0f, -5.0f }; // カメラの位置

    // === 描画処理の有効・無効 ===
    RenderLayerMask cullingMask = RENDER_LAYER_MASK_ALL;
    RenderLayerMask maskCullingMask = RenderLayerToMask(RenderLayer::Player);

public:
    // カメラの位置、注視点、上方ベクトルの設定・取得
    void    SetAtPosition(XMFLOAT3 atPosition) { m_atPosition = atPosition; }
    XMFLOAT3   GetAtPosition() const { return m_atPosition; }
    XMFLOAT3   GetUpVector() const { return m_upVector; }

    // カメラのFOV設定・取得
    void    SetFov(float fov) { m_fov = fov; }
    float   GetFov() const { return m_fov; }
    // カメラのアスペクト比設定・取得
    void    SetAspect(float aspect) { m_aspect = aspect; }
    float   GetAspect() const { return m_aspect; }
    // カメラのクリップ距離設定・取得
    void    SetNearClip(float nearClip) { m_nearClip = nearClip; }
    void    SetFarClip(float farClip) { m_farClip = farClip; }
    float   GetNearClip() const { return m_nearClip; }
    float   GetFarClip() const { return m_farClip; }

    // 描画処理の有効・無効設定・取得
    void    SetCullingMask(RenderLayerMask mask) { cullingMask = mask; }
    RenderLayerMask GetCullingMask() const { return cullingMask; }
    void    SetMaskCullingMask(RenderLayerMask mask) { maskCullingMask = mask; }
    RenderLayerMask GetMaskCullingMask() const { return maskCullingMask; }

    // ビュー行列の取得
    XMMATRIX    GetViewMatrix() const { return m_view; }
    // プロジェクション行列の取得
    XMMATRIX    GetProjectionMatrix() const { return m_projection; }
    // カメラの位置の取得
    XMFLOAT3    GetEyePosition() const { return m_eyePosition; }

    // カメラ正面方向の取得
    XMFLOAT3    GetForward() const {
        XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&m_atPosition), XMLoadFloat3(&m_eyePosition)));
        XMFLOAT3 forwardDir;
        XMStoreFloat3(&forwardDir, forward);
        return forwardDir;
    }
    // カメラ右方向の取得
    XMFLOAT3    GetRight() const {
        XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&m_atPosition), XMLoadFloat3(&m_eyePosition)));
        XMVECTOR up = XMLoadFloat3(&m_upVector);
        XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));
        XMFLOAT3 rightDir;
        XMStoreFloat3(&rightDir, right);
        return rightDir;
    }

private:
    friend CameraProcessor;
    void    SetViewMatrix(XMMATRIX view) { m_view = view; }
    void    SetProjectionMatrix(XMMATRIX projection) { m_projection = projection; }
    void    SetEyePosition(XMFLOAT3 eyePosition) { m_eyePosition = eyePosition; }

};

#endif // CAMERA_COMPONENT_H