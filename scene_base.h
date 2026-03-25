//---------------------------------------------------
// scene_base.h
// 
// ・Sceneの基底クラス。
// ・データの実体やアドレスを管理する。
// 
// Author：Miu Kitamura
// Date  ：2025/12/11
//---------------------------------------------------
#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include <iostream>
#include <vector>
#include "scene_interface.h"
#include "game_object.h"

class SceneBase : public IScene {
private:
    static constexpr int    MAX_GAMEOBJECTS = 1024;  // シーン内の最大GameObject数
    static constexpr int    MAX_COMPONENTPOOLS = 64; // シーン内の最大ComponentPool数

    // GameObjectリスト
    std::vector<GameObject>     m_gameObjects = {};
    int                     m_gameObjectCount = 0;  // 生成したGameObject数カウンタ

    // ComponentPoolリスト
    std::vector<std::unique_ptr<IComponentPool>> m_componentPools;

    // 空きスロット管理用リスト
    std::vector<size_t>         m_freeGameObjectIndices = {};

protected: // IScene implementation
    std::vector<std::unique_ptr<IComponentPool>>& ComponentPools() override {
        return m_componentPools;
    }

public:
    SceneBase() {
        m_gameObjects.clear();
        m_gameObjects.reserve(MAX_GAMEOBJECTS);
        m_componentPools.clear();
        m_componentPools.reserve(MAX_COMPONENTPOOLS);
        m_freeGameObjectIndices.clear();
    }

    virtual void   Initialize() override = 0;
    virtual void   Finalize() override = 0;
    virtual void   Update() override = 0;
    virtual void   Draw() override = 0;

    void Reset() {
        m_gameObjects.clear();
        m_gameObjects.reserve(MAX_GAMEOBJECTS);
        m_componentPools.clear();
        m_componentPools.reserve(MAX_COMPONENTPOOLS);
        m_freeGameObjectIndices.clear();
    }

    // GameObjectの生成
    // ・return: 生成したGameObjectへのポインタ
    GameObject* CreateGameObject() override {
        assert(m_gameObjects.size() < MAX_GAMEOBJECTS && "IScene has reached its maximum GameObject capacity.");

        // 空きスロットがあればそこを利用
        if (!m_freeGameObjectIndices.empty()) {
            size_t index = m_freeGameObjectIndices.back();
            m_freeGameObjectIndices.pop_back();

            m_gameObjects[index] = GameObject();
            GameObject* pGameObject = &m_gameObjects[index];
            pGameObject->SetID(m_gameObjectCount++);
            pGameObject->SetScene(this);

            return pGameObject;
        }

        GameObject* pGameObject = &(m_gameObjects.emplace_back());
        pGameObject->SetID(m_gameObjectCount++);
        pGameObject->SetScene(this);

        return pGameObject;
    }

    // GameObjectリストの取得
    std::vector<GameObject>& GetGameObjects() override { return m_gameObjects; }

    // GameObjectの破棄
    void    CollectDestroyedGameObjects() {
        for (int i = 0; i < m_gameObjects.size(); i++) {
            GameObject& obj = m_gameObjects[i];
            if (!obj.m_isDestroy)continue;

            // ComponentPoolからComponentを削除
            for (auto& pool : m_componentPools) {
                pool->Remove(obj.GetID());
            }

            // GameObjectの終了処理
            obj.FinalizeInternal();

            // 空きスロットとして管理リストに追加
            m_freeGameObjectIndices.push_back(i);
        }
    }

    // GameObjectの取得
    GameObject* GetGameObjectByID(unsigned int id) override {
        for (auto& obj : m_gameObjects) {
            if (obj.GetID() == id) {
                return &obj;
            }
        }
        return nullptr;
    }

    GameObject* GetGameObjectByName(const std::string& name) override {
        for (auto& obj : m_gameObjects) {
            if (obj.GetName() == name) {
                return &obj;
            }
        }
        return nullptr;
    }

    // ComponentPoolの取得
    std::vector<std::unique_ptr<IComponentPool>>& GetComponentPools() override {
        return m_componentPools;
    }
};

#endif