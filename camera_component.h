//---------------------------------------------------
// camera_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/29
//---------------------------------------------------
#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "component.h"

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

    // シェーダー
    float       m_shaderGrayRate = 0.0f;
    XMFLOAT4    m_clearColor = { 0.1f,0.7f,1.0f,1.0f }; // クリアカラー

public:
    void    SetAtPosition(XMFLOAT3 atPosition) { m_atPosition = atPosition; }
    XMFLOAT3   GetAtPosition() const { return m_atPosition; }
    XMFLOAT3   GetUpVector() const { return m_upVector; }

    void    SetFov(float fov) { m_fov = fov; }
    void    SetAspect(float aspect) { m_aspect = aspect; }
    void    SetNearClip(float nearClip) { m_nearClip = nearClip; }
    void    SetFarClip(float farClip) { m_farClip = farClip; }

    float   GetFov() const { return m_fov; }
    float   GetAspect() const { return m_aspect; }
    float   GetNearClip() const { return m_nearClip; }
    float   GetFarClip() const { return m_farClip; }

    XMMATRIX    GetViewMatrix() const { return m_view; }
    XMMATRIX    GetProjectionMatrix() const { return m_projection; }
    XMFLOAT3    GetEyePosition() const { return m_eyePosition; }

private:
    friend CameraProcessor;
    void    SetViewMatrix(XMMATRIX view) { m_view = view; }
    void    SetProjectionMatrix(XMMATRIX projection) { m_projection = projection; }
    void    SetEyePosition(XMFLOAT3 eyePosition) { m_eyePosition = eyePosition; }

    ID3D11ShaderResourceView* m_snapshot = nullptr; // スナップショット用テクスチャSRV
    void    SetSnapshot(ID3D11ShaderResourceView* snapshot) { m_snapshot = snapshot; }

public:
    ID3D11ShaderResourceView* GetSnapshot() const { return m_snapshot; }

    void    SetClearColor(XMFLOAT4 clearColor) { m_clearColor = clearColor; }
    XMFLOAT4   GetClearColor() const { return m_clearColor; }

};

#endif // CAMERA_COMPONENT_H