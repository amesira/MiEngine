//===================================================
// collider_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#ifndef COLLIDER_COMPONENT_H
#define COLLIDER_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

#include "component.h"

class ColliderComponent :public Component {
public:
    enum class Shape {
        Box,
        Sphere,
    };
    struct CollisionData {
        ColliderComponent*  m_other = nullptr;  // 衝突相手のコライダーコンポーネント
        XMFLOAT3    m_mtv = { 0.0f,0.0f,0.0f }; // 最小移動ベクトル

        bool    m_isCollision = false;  // 当たり判定フラグ
        bool    m_wasCollision = false; // 前フレームの当たり判定フラグ

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
    enum class Layer {
        Default = 0,
        Field,
        Player,
        PlayerOnBall,
        Ball,
        Enemy,

        MAX,
    };

    // 衝突情報の最大登録数
    static const int MAX_COLLISION_DATA = 8;

protected:
    Shape m_shape;
    DirectX::XMFLOAT3 m_center = { 0.0f,0.0f,0.0f };

private:
    Layer m_layer = Layer::Default;
    CollisionData m_collisionData[MAX_COLLISION_DATA];
    
public:
    ~ColliderComponent() = default;

    // コライダーの形状取得
    Shape   GetShape()const { return m_shape; }

    void    SetCenter(DirectX::XMFLOAT3 center) { m_center = center; }
    DirectX::XMFLOAT3   GetCenter()const { return m_center; }

    void    SetLayer(Layer layer) { m_layer = layer; }
    Layer   GetLayer()const { return m_layer; }

    CollisionData GetCollisionData(int index) const {
        if (index < 0 || index >= MAX_COLLISION_DATA) {
            return CollisionData{};
        }
        return m_collisionData[index];
    }

    void    UpdateCollisionData() {
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_other) {
                data.m_wasCollision = data.m_isCollision;
                data.m_isCollision = false;
                if (!data.m_wasCollision)data.m_other = nullptr;
            }
        }
    }

    // 衝突情報登録
    int     RegisterCollisionData(ColliderComponent* other, DirectX::XMFLOAT3 mtv) {
        CollisionData* slot = nullptr;

        for(int i = 0; i < MAX_COLLISION_DATA; i++){
            CollisionData& data = m_collisionData[i];
            if(data.m_other == other) {
                slot = &data;
                break;
            }
            else if (data.m_other == nullptr) {
                slot = &data;
            }
        }

        if (slot) {
            slot->m_other = other;
            slot->m_mtv = mtv;
            slot->m_isCollision = true;
            return 1;
        }

        return 0;
    }

    // 最小移動ベクトルの最小値取得
    DirectX::XMFLOAT3& GetMinMtv() {
        DirectX::XMFLOAT3 minMtv = { 0.0f,0.0f,0.0f };
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_isCollision && data.m_other) {
                if (data.m_mtv.x < minMtv.x) {
                    minMtv.x = data.m_mtv.x;
                }
                if (data.m_mtv.y < minMtv.y) {
                    minMtv.y = data.m_mtv.y;
                }
                if (data.m_mtv.z < minMtv.z) {
                    minMtv.z = data.m_mtv.z;
                }
            }
        }
        return minMtv;
    }

    // 最小移動ベクトルの最大値取得
    DirectX::XMFLOAT3& GetMaxMtv() {
        DirectX::XMFLOAT3 maxMtv = { 0.0f,0.0f,0.0f };
        for (int i = 0; i < MAX_COLLISION_DATA; i++) {
            CollisionData& data = m_collisionData[i];
            if (data.m_isCollision && data.m_other) {
                if (data.m_mtv.x > maxMtv.x) {
                    maxMtv.x = data.m_mtv.x;
                }
                if (data.m_mtv.y > maxMtv.y) {
                    maxMtv.y = data.m_mtv.y;
                }
                if (data.m_mtv.z > maxMtv.z) {
                    maxMtv.z = data.m_mtv.z;
                }
            }
        }
        return maxMtv;
    }
};

class BoxColliderComponent :public ColliderComponent {
private:
    DirectX::XMFLOAT3   m_scale = { 1.0f,1.0f,1.0f };

public:
    BoxColliderComponent() {  m_shape = ColliderComponent::Shape::Box; }

    void    SetScale(DirectX::XMFLOAT3 scale) {  m_scale = scale;}
    DirectX::XMFLOAT3   GetScale() { return m_scale; }
};

class SphereColliderComponent :public ColliderComponent {
private:
    float   m_radius = 0.5f;

public:
    SphereColliderComponent() { m_shape = ColliderComponent::Shape::Sphere; }

    void    SetRadius(float radius) { m_radius = radius; }
    float   GetRadius() { return m_radius; }

};

#endif