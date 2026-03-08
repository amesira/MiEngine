//===================================================
// behavior.h [Behavior基底クラス]
// 
// ・データと処理の両方を含み、これ一つで処理が完結するもの。
// 　（Unityで言うMonoBehaviorクラスである）
// ・内部で設定したいComponentをいくつか取得しておき、それを操作することで
// 　振る舞いを表す。（SetPosition()など）
// 
// ・Componentを作るか、Behaviorを作るかは、
// 　処理の順番の重要さ、呼び出しの頻繁さ、Unityではどうなっているかなどで判断する。
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef BEHAVIOR_H
#define BEHAVIOR_H

// 自らのオーナーとして保持するため、前方宣言を行う。
class GameObject;
class IScene;

class Behavior {
private:
    int         m_typeID = -1;

    bool        m_enable = true;
    GameObject* m_pOwner = nullptr;

public:
    Behavior(int typeID) : m_typeID(typeID) {}
    virtual ~Behavior() = default;

    virtual void    Start(IScene* pScene) {}
    virtual void    Update(IScene* pScene) {}

    // Behaviorのタイプ取得
    int     GetType() const { return m_typeID; }

    // Behaviorのオーナー取得
    GameObject* GetOwner() const { return m_pOwner; }

    // enableの設定・取得 
    void    SetEnable(bool enable) { m_enable = enable; }
    bool    GetEnable() const { return m_enable; }

private:
    // オーナー設定
    friend GameObject;
    void    SetOwner(GameObject* pGameObject) { m_pOwner = pGameObject; }
};

#endif