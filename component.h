//===================================================
// component.h [component基底クラス]
// 
// ・処理に用いるデータ群。TransformComponentはこれを継承して作成する。
// ・変数、セッター、ゲッターで主に構成される。
// 
// ・Componentを作るか、Behaviorを作るかは、
// 　処理の順番の重要さ、呼び出しの頻繁さ、Unityではどうなっているかなどで判断する。
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef COMPONENT_H
#define COMPONENT_H

class GameObject;

class Component {
private:
    bool        m_enable = true;
    GameObject* m_pOwner = nullptr;

public:
    Component() = default;
    virtual ~Component() = default;

    // Componentのオーナー取得
    GameObject* GetOwner() const{ return m_pOwner; }

    // enableの設定・取得 
    void    SetEnable(bool enable) { m_enable = enable; }
    bool    GetEnable() const { return m_enable; }

private:
    // オーナー設定が出来るのはGameObjectだけ。
    friend class GameObject;
    void    SetOwner(GameObject* pGameObject) { m_pOwner = pGameObject; }

};


#endif