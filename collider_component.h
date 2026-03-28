//===================================================
// collider_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#ifndef COLLIDER_COMPONENT_H
#define COLLIDER_COMPONENT_H
#include "component.h"

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

#include <array>

class ColliderComponent :public Component {
public:
    // レイヤー
    enum class Layer {
        Default = 0,
        Field,
        Player,
        PlayerOnBall,
        Ball,
        Enemy,

        MAX,
    };

    // 衝突情報構造体
    struct CollisionData {
        ColliderComponent*  m_other = nullptr;  // 衝突相手のコライダーコンポーネント
        
        XMFLOAT3    m_mtv = { 0.0f,0.0f,0.0f }; // 最小移動ベクトル
        XMFLOAT3    m_correction = { 0.0f,0.0f,0.0f }; // 実際の補正値

    protected:
        friend class ColliderComponent;
        friend class CollisionPass;
        friend class ResolvePass;
        bool    m_isCollision = false;  // 当たり判定フラグ
        bool    m_wasCollision = false; // 前フレームの当たり判定フラグ

    public:
        bool    GetCollisionEnter() {
            if(m_other == nullptr)return false;
            return m_isCollision && !m_wasCollision;
        }
        bool    GetCollisionStay() { 
            if (m_other == nullptr)return false; 
            return m_isCollision;
        }
        bool    GetCollisionExit() { 
            if (m_other == nullptr)return false;
            return !m_isCollision && m_wasCollision; 
        }
    };
    // 衝突情報の最大登録数
    static const int MAX_COLLISION_DATA = 8;

private:
    // レイヤー設定
    Layer m_layer = Layer::Default;
    // 補正値を作成するかどうかのフラグ（自分も相手も）
    bool m_createCorrection = true;

    // コライダーの中心位置
    XMFLOAT3 m_center = { 0.0f,0.0f,0.0f };
    
    // 衝突情報の配列
    std::array<CollisionData, MAX_COLLISION_DATA> m_collisionData;
    // 連続衝突判定のためのステップ数
    int m_ccbStep = 0;

public:
    ~ColliderComponent() = default;

    // レイヤー設定・取得
    void    SetLayer(Layer layer) { m_layer = layer; }
    Layer   GetLayer()const { return m_layer; }
    // 補正値作成フラグ設定・取得
    void    SetCreateCorrection(bool create) { m_createCorrection = create; }
    bool    GetCreateCorrection() const { return m_createCorrection; }

    // コライダーの中心位置設定・取得
    void    SetCenter(XMFLOAT3 center) { m_center = center; }
    XMFLOAT3   GetCenter()const { return m_center; }

    // 衝突情報の取得
    CollisionData GetCollisionDataSlot(int index) const {
        if (index >= 0 && index < MAX_COLLISION_DATA) {
            return m_collisionData[index];
        }
        return CollisionData{};
    }

    // 連続衝突判定のステップ数の取得
    int GetCCBStep() const { return m_ccbStep; }

private:
    // 衝突情報の更新
    void    UpdateCollisionData() {
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_other == nullptr) continue;

            data.m_wasCollision = data.m_isCollision;
            data.m_isCollision = false;
            if (!data.m_wasCollision)data.m_other = nullptr;
        }
    }
    // 衝突情報登録
    ColliderComponent::CollisionData* RegisterCollisionData(ColliderComponent* other) {
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_other != other) continue;

            data.m_isCollision = true;
            return &data;
        }
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_other != nullptr) continue;

            data.m_other = other;
            data.m_isCollision = true;
            return &data;
        }
        return nullptr;
    }
    // 衝突情報配列の取得
    std::array<CollisionData, MAX_COLLISION_DATA>& GetCollisionData() { return m_collisionData; }

    // 連続衝突判定のステップ数の設定
    void SetCCBStep(int step) { m_ccbStep = step; }

public:
    // 内部アクセス用クラス
    class Internal {
        friend class CollisionPass;
        friend class ResolvePass;
    public:
        // 衝突情報の更新
        static void UpdateCollisionData(ColliderComponent* collider) {
            collider->UpdateCollisionData();
        }
        // 衝突情報登録
        static ColliderComponent::CollisionData* RegisterCollisionData(ColliderComponent* collider, ColliderComponent* other) {
            return collider->RegisterCollisionData(other);
        }
        // 衝突情報配列の取得
        static std::array<CollisionData, MAX_COLLISION_DATA>& GetCollisionData(ColliderComponent* collider) {
            return collider->GetCollisionData();
        }
        // 連続衝突判定のステップ数の設定
        static void SetCCBStep(ColliderComponent* collider, int step) {
            collider->SetCCBStep(step);
        }
    };

};

//----------------------------------------------------
// BoxCollider
//----------------------------------------------------
class BoxColliderComponent :public ColliderComponent {
private:
    XMFLOAT3   m_scale = { 1.0f,1.0f,1.0f };

public:
    BoxColliderComponent() {}

    void    SetScale(DirectX::XMFLOAT3 scale) {  m_scale = scale;}
    DirectX::XMFLOAT3   GetScale() { return m_scale; }
};

//----------------------------------------------------
// SphereCollider
//----------------------------------------------------
class SphereColliderComponent :public ColliderComponent {
private:
    float   m_radius = 0.5f;

public:
    SphereColliderComponent() { }

    void    SetRadius(float radius) { m_radius = radius; }
    float   GetRadius() { return m_radius; }

};

#endif