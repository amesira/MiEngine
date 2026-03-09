//---------------------------------------------------
// component_pool.h
// 
// ・型ごとにComponentを管理するプール。
// 
// Author：Miu Kitamura
// Date  ：2025/12/07
//---------------------------------------------------
#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <vector>
#include <assert.h>

#include "component_type_id.h"
#include "component_pool_interface.h"

class GameObject;

template <class T>
class ComponentPool : public IComponentPool {
private:
    static constexpr size_t COMPONENTS_MAX = 1024;

    // Componentリスト
    std::vector<T>              m_components = {};

    // GameObjectのIDリスト
    // ・m_componentsとインデックスを対応させる
    std::vector<unsigned int>   m_gameObjectIDs = {};

    // 空きスロット管理用リスト
    std::vector<size_t>         m_freeIndices = {};

public:
    ComponentPool() : IComponentPool(ComponentTypeID::getTypeID<T>(), ComponentTypeID::getBaseTypeID<T>()) {
        m_components.reserve(COMPONENTS_MAX);
        m_gameObjectIDs.reserve(COMPONENTS_MAX);
        m_freeIndices.clear();
    }

    // Componentを生成してComponentPoolに追加
    // ・pGameObject: Componentを所有するGameObjectへのポインタ
    T*      Create(unsigned int gameObjectID) {
        assert(m_components.size() < COMPONENTS_MAX && "ComponentPool has reached its maximum capacity.");
        
        // 空きスロットがあればそこを利用
        // ・Componentのメンバ変数に参照型やポインタがある場合、T()でエラーが出る可能性があるため注意
        if (!m_freeIndices.empty()) {
            size_t index = m_freeIndices.back();
            m_freeIndices.pop_back();
            m_components[index] = T();
            m_gameObjectIDs[index] = gameObjectID;
            return &m_components[index];
        }

        m_components.emplace_back();
        m_gameObjectIDs.push_back(gameObjectID);

        return &m_components.back();
    }

    // ComponentPoolからComponentを削除
    // ・gameObjectID: 削除するComponentを所有するGameObjectのID
    void    Remove(unsigned int gameObjectID) override {
        for (int i = 0; i < m_components.size(); i++) {
            unsigned int id = m_gameObjectIDs[i];
            // 指定されたGameObjectIDと一致したら削除
            if (id == gameObjectID) {
                // 空きスロットとして管理リストに追加
                m_freeIndices.push_back(i);

                m_components[i].SetEnable(false); // 無効化しておく
                m_gameObjectIDs[i] = -1; // 無効なIDにしておく

                return;
            }
        }
    }

    // GameObjectIDからComponentを取得
    // ・gameObjectID: 取得するComponentを所有するGameObjectのID
    T*  GetByGameObjectID(unsigned int gameObjectID) {
        for (int i = 0; i < m_components.size(); i++) {
            unsigned int id = m_gameObjectIDs[i];
            // 指定されたGameObjectIDと一致したらComponentを返す
            if (id == gameObjectID) {
                return &m_components[i];
            }
        }
        return nullptr;
    }

    // ComponentPool内のComponentリストを取得
    std::vector<T>&     GetList() { return m_components; }

    // ComponentPool内のComponentに対してfuncを呼び出す
    void    Foreach(std::function<void(Component* c)> func) override {
        for (int i = 0; i < m_components.size(); i++) {
            func(&m_components[i]);
        }
    }

};

#endif