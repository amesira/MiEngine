//---------------------------------------------------
// behavior_component.h
// 
// ・データと処理の両方を含み、これ一つで処理が完結するもの。
// 　（Unityで言うMonoBehaviorクラスである）
// ・内部で設定したいComponentをいくつか取得しておき、それを操作することで
// 　振る舞いを表す。（SetPosition()など）
// 
// ・Componentを作るか、Behaviorを作るかは、
// 　処理の順番の重要さ、呼び出しの頻繁さ、Unityではどうなっているかなどで判断する。
// 
// 2026/03/09 改修
// ・Behaviorクラスを規定とするのではなく、Componentクラスを継承してBehaviorComponentクラスを作成する形に変更。
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef BEHAVIOR_COMPONENT_H
#define BEHAVIOR_COMPONENT_H
#include "component.h"

class BehaviorComponent : public Component {
private:
    friend class BehaviorProcessor;
    bool m_hasStarted = false; // Start()が呼び出されたかどうかのフラグ

public:
    virtual ~BehaviorComponent() = default;

    virtual void    Start() = 0;
    virtual void    Update() = 0;

    virtual void    DrawComponentInspector() = 0;

};

#endif // BEHAVIOR_COMPONENT_H