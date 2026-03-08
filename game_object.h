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

#include "behavior.h"

#include "scene_interface.h"

class GameObject {
private:
    IScene*         m_pScene = nullptr; // 所属するシーンへのポインタ
    unsigned int    m_id = -1;           // GameObjectのID

    std::string     m_name = "None";    // GameObjectの名前
    bool            m_active = true;    // アクティブフラグ
    bool            m_isDestroy = false;// 破棄予約フラグ
    
    // Behaviorリスト
    std::vector<std::unique_ptr<Behavior>>  m_pBehaviors = {};

public:
    // GameObjectの更新処理
    // ・更新毎、Behavior.Update()を呼び出す。
    void    Update() {
        if (!m_active || m_isDestroy)return;
        for (auto& be : m_pBehaviors) {
            if (!be.get()->GetEnable())continue;
            be.get()->Update(m_pScene);
        }
    }

private:
    // SceneBaseからSetScene, SetIDを呼び出せるようにする。
    friend class SceneBase;
    void    SetScene(IScene* pScene) { m_pScene = pScene; }
    void    SetID(unsigned int id) { m_id = id; }

    // 内部終了処理
    void    FinalizeInternal() {
        m_id = -1;

        m_name = "None";
        m_active = false;
        m_isDestroy = false;

        m_pBehaviors.clear();
    }

public:
    IScene*         GetScene() const { return m_pScene; }
    unsigned int    GetID() const { return m_id; }

    void    SetName(const std::string& name) { m_name = name; }
    void    SetActive(bool active) { m_active = active; }

    std::string     GetName() const { return m_name; }
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
        return nullptr;
    }

    // Behaviorの追加
    template<class T>
    T*  AddBehavior() {
        // 引数ownerはGameObjectのポインタを渡す。
        m_pBehaviors.push_back(std::make_unique<T>(this));
        auto* pBe = static_cast<T*>(m_pBehaviors.back().get());
        pBe->SetOwner(this);
        return pBe;
    }

    // Behaviorの取得
    template<class T>
    T* GetBehavior() {
        for (int i = 0; i < m_pBehaviors.size(); i++) {
            Behavior* pBe = m_pBehaviors[i].get();
            if (pBe->GetType() == BehaviorTypeID::getTypeID<T>()) {
                return static_cast<T*>(pBe);
            }
        }
        return nullptr;
    }
};

#endif


