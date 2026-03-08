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

class IComponentPool {
private:
    int m_typeId = -1;

public:
    // コンストラクタで型ごとに一意なIDを設定
    IComponentPool(int id) : m_typeId(id) {}
    virtual ~IComponentPool() = default;

    // コンポーネントIDの取得
    int     GetTypeID() const { return m_typeId; }

    // コンポーネントの削除
    virtual void    Remove(unsigned int gameObjectID) = 0;
};

#endif