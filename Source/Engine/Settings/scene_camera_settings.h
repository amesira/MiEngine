//---------------------------------------------------
// scene_camera_settings.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/30
//---------------------------------------------------
#ifndef SCENE_CAMERA_SETTINGS_H
#define SCENE_CAMERA_SETTINGS_H
#include <DirectXMath.h>
using namespace DirectX;

// シーン全体のカメラ設定を保持するクラス
class SceneCameraSettings {
private:
    XMFLOAT3    m_position = { 0.0f, 0.0f, -5.0f }; // カメラの位置
    XMFLOAT3    m_atPosition = { 0.0f, 0.0f, 0.0f };// 注視点
    XMFLOAT3 m_upVector = { 0.0f, 1.0f, 0.0f };  // 上方ベクトル

    float       m_fov = 60.0f;              // 視野角（画角）
    float       m_aspect = 16.0f / 9.0f;    // 画面のアスペクト比
    float       m_nearClip = 0.1f;          // 近面クリップ距離
    float       m_farClip = 300.0f;         // 遠面クリップ距離

    XMMATRIX    m_view = XMMatrixIdentity();         // ビュー行列
    XMMATRIX    m_projection = XMMatrixIdentity();   // プロジェクション行列

public:
    // カメラ位置の取得・設定
    void    SetPosition(XMFLOAT3 position) { m_position = position; }
    XMFLOAT3   GetPosition() const { return m_position; }
    // 注視点の取得・設定
    void    SetAtPosition(XMFLOAT3 atPosition) { m_atPosition = atPosition; }
    XMFLOAT3   GetAtPosition() const { return m_atPosition; }

    // 視野角の取得・設定
    void    SetFov(float fov) { m_fov = fov; }
    float   GetFov() const { return m_fov; }
    // アスペクト比の取得・設定
    void    SetAspect(float aspect) { m_aspect = aspect; }
    float   GetAspect() const { return m_aspect; }
    // 近面クリップ距離の取得・設定
    void    SetNearClip(float nearClip) { m_nearClip = nearClip; }
    float   GetNearClip() const { return m_nearClip; }
    // 遠面クリップ距離の取得・設定
    void    SetFarClip(float farClip) { m_farClip = farClip; }
    float   GetFarClip() const { return m_farClip; }

    // ビュー行列の更新・取得
    void UpdateViewMatrix() {
        m_view = XMMatrixLookAtLH(
            XMLoadFloat3(&m_position),
            XMLoadFloat3(&m_atPosition),
            XMLoadFloat3(&m_upVector));
    }
    XMMATRIX GetViewMatrix() const { return m_view; }
    // プロジェクション行列の更新・取得
    void UpdateProjectionMatrix() {
        m_projection = XMMatrixPerspectiveFovLH(
            XMConvertToRadians(m_fov),
            m_aspect,
            m_nearClip,
            m_farClip);
    }
    XMMATRIX GetProjectionMatrix() const { return m_projection; }

};

#endif // SCENE_SETTINGS_H