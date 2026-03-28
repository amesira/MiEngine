//===================================================
// game_object.h [ゲームオブジェクト]
// 
// ・Component, Behavior のリストを保持する箱。
// ・Player, Enemy などはすべてこの箱に何を入れるかで構成される。
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>
#include <vector>
#include <string>

#include "component.h"
#include "component_pool.h"

#include "scene_interface.h"

class GameObject {
private:
    IScene*         m_pScene = nullptr; // 所属するシーンへのポインタ
    unsigned int    m_id = -1;           // GameObjectのID

    std::string     m_name = "GameObject";    // GameObjectの名前
    bool            m_active = true;    // アクティブフラグ
    bool            m_isDestroy = false;// 破棄予約フラグ
    
private:
    // SceneBaseからSetScene, SetIDを呼び出せるようにする。
    friend class SceneBase;
    void    SetScene(IScene* pScene) { m_pScene = pScene; }
    void    SetID(unsigned int id) { m_id = id; }

    // 内部終了処理
    void    FinalizeInternal() {
        m_id = -1;

        m_name = "GameObject";
        m_active = false;
        m_isDestroy = false;
    }

public:
    // シーンの取得
    IScene*         GetScene() const { return m_pScene; }
    // IDの取得
    unsigned int    GetID() const { return m_id; }

    // 名前の設定・取得
    void    SetName(const std::string& name) { m_name = name; }
    std::string     GetName() const { return m_name; }

    // アクティブの設定・取得
    void    SetActive(bool active) { m_active = active; }
    bool            GetActive() { return m_active; }

    // GameObjectの破棄予約
    void    Destroy() {
        if(m_isDestroy)return;
        m_isDestroy = true;
    }

    // Componentの追加
    template<class T>
    T* AddComponent() {
        auto* compPool = m_pScene->GetComponentPool<T>();
        if (!compPool) {
            compPool = m_pScene->AddComponentPool<T>();
        }

        // Componentを生成してComponentPoolに追加
        T* component = compPool->Create(m_id);
        if (component) {
            component->SetOwner(this);
            return component;
        }

        return nullptr;
    }

    // Componentの取得
    template<class T>
    T* GetComponent(){
        auto* compPool = m_pScene->GetComponentPool<T>();
        if (compPool) {
            return compPool->GetByGameObjectID(m_id);
        }
        else {
            // GetComponentsByBaseTypeから取得する
        }
        return nullptr;
    }

    // GameObjectが持つすべてのComponentを取得
    std::vector<Component*> GetAllComponents(){
        std::vector<Component*> result;

        if (!m_pScene) return result;

        auto& pools = m_pScene->GetComponentPools();
        for (auto& pool : pools) {
            if (!pool) continue;

            Component* comp = pool->GetComponentInterface(m_id);
            if (comp) {
                result.push_back(comp);
            }
        }
        return result;
    }

    // GameObjectが持つ全てのBehaviorComponentを取得
    std::vector<BehaviorComponent*> GetBehaviorComponents() {
        std::vector<BehaviorComponent*> result;

        if (!m_pScene) return result;

        auto& pools = m_pScene->GetComponentPools();
        for (auto& pool : pools) {
            if (!pool) continue;

            // ベースとなる型がBehaviorComponentのComponentPoolを探す
            if (pool->GetBaseTypeID() != ComponentTypeID::getTypeID<BehaviorComponent>()) continue;

            // そのComponentPoolからGameObjectが持つBehaviorComponentを取得
            pool->Foreach([this, &result](Component* c) {
                if (c->GetOwner()->GetID() == m_id) {
                    result.push_back(static_cast<BehaviorComponent*>(c));
                }
            });
        }
        return result;
    }

};

#endif


