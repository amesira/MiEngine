//---------------------------------------------------
// animation_component.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/06
//---------------------------------------------------
#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H
#include "component.h"
#include <string>
#include <vector>
#include "model_resource.h"

// アニメーションの状態を表す構造体
struct AnimationState {
    std::string clipName;
    int         clipIndex = -1;
    float   timer = 0.0f;
    float   speed = 1.0f; // 再生速度の倍率
};

// アニメーションのトランジションを表す構造体
struct AnimationTransition {
    AnimationState* fromState = nullptr;
    AnimationState* toState = nullptr;

    float duration = 0.5f; // トランジションの長さ（秒）
    float timer = 0.0f;    // トランジションの経過時間
    bool isTransitioning = false; // トランジション中かどうか
};

class AnimationComponent : public Component {
private:
    AnimationState m_currentState; // 現在のアニメーション状態

public:
    static constexpr char CLIP_NONE[] = "None";

    // アニメーション状態の設定・取得
    void SetAnimationState(int clipIndex, float speed) {
        if (m_currentState.clipIndex == clipIndex) return; // すでに同じクリップが再生中なら何もしない
        m_currentState.clipName = "aa";
        m_currentState.clipIndex = clipIndex;
        m_currentState.timer = 0.0f;
        m_currentState.speed = speed;
    }
    AnimationState& GetAnimationState() { return m_currentState; }

};

#endif // ANIMATION_COMPONENT_H