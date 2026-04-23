//---------------------------------------------------
// component_pool_interface.h
// 
// ・ComponentPoolのインターフェース。
// ・型ごとに異なるComponentPoolを同じコンテナで管理するために用いる。
// 
// ・IComponentPoolを継承したComponentPoolは、型ごとに一意なIDを持つ。
// 
// Author：Miu Kitamura
// Date  ：2025/12/11
//---------------------------------------------------
#ifndef COMPONENT_POOL_INTERFACE_H
#define COMPONENT_POOL_INTERFACE_H
#include <functional>
class Component;

class IComponentPool {
private:
    int m_typeId = -1;
    int m_baseTypeId = -1; // ベースとなる型のID（継承関係を確認するために使用）

public:
    // コンストラクタで型ごとに一意なIDを設定
    IComponentPool(int id, int baseId) : m_typeId(id), m_baseTypeId(baseId) {}
    virtual ~IComponentPool() = default;

    // コンポーネントIDの取得
    int     GetTypeID() const { return m_typeId; }
    // ベースとなる型のIDの取得
    int     GetBaseTypeID() const { return m_baseTypeId; }

    // コンポーネントの削除
    virtual void    Remove(unsigned int gameObjectID) = 0;

    // コンポーネントに対して関数を適用
    virtual void    Foreach(std::function<void(Component* c)> func) = 0;

    // Component基底クラスの取得
    virtual Component* GetComponentInterface(unsigned int gameObjectID) = 0;

};

#endif