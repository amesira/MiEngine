//---------------------------------------------------
// sprite_animation_component.h
// 
// Author：Miu Kitamura
// Date  ：2026/05/29
//---------------------------------------------------
#ifndef SPRITE_ANIMATION_COMPONENT_H
#define SPRITE_ANIMATION_COMPONENT_H
#include "Engine/Core/component.h"

#include "Engine/Device/direct3d.h"
using namespace DirectX;
#include <string>
#include <vector>

#include "Engine/Graphics/texture_resource.h"

class SpriteAnimationComponent : public Component {
public:
    // スプライトアニメーションフレーム
    struct Frame {
        TextureResource* textureResource = nullptr;
        XMFLOAT4    uvRect = { 0.0f,0.0f,1.0f,1.0f };
        XMFLOAT4    color = { 1.0f,1.0f,1.0f,1.0f };
        float       duration = 0.1f; // フレームの表示時間（秒）
    };
    // スプライトアニメーションクリップ
    struct Clip {
        std::string name; // クリップ名
        std::vector<Frame> frames;
        float speed = 1.0f; // アニメーションの再生速度
        bool loop = true;  // ループ再生するか
    };

private:
    friend class SpriteAnimationProcessor;
    std::vector<Clip> m_clips; // アニメーションクリップのリスト

    int m_currentClipIndex = -1; // 現在再生中のクリップのインデックス
    int m_currentFrameIndex = 0; // 現在再生中のフレームのインデックス

    float m_frameTimer = 0.0f; // 現在のフレームの経過時間

    bool m_isPlaying = false; // アニメーションが再生中かどうか

public:
    // クリップの追加
    void AddClip(const Clip& clip) { m_clips.push_back(clip); }
    // クリップの取得
    const Clip* GetClip(const std::string& clipName, int* outIndex = nullptr) const {
        for (size_t i = 0; i < m_clips.size(); i++) {
            if (m_clips[i].name != clipName) continue;
                
            if (outIndex) *outIndex = static_cast<int>(i);
            return &m_clips[i];
        }
        return nullptr;
    }
    const Clip* GetCurrentClip() const {
        if (m_currentClipIndex >= 0 && m_currentClipIndex < m_clips.size()) {
            return &m_clips[m_currentClipIndex];
        }
        return nullptr;
    }

    // クリップの再生
    void PlayClip(int clipIndex) {
        if (clipIndex >= 0 && clipIndex < m_clips.size()) {
            m_currentClipIndex = clipIndex;
            m_currentFrameIndex = 0;
            m_frameTimer = 0.0f;
            m_isPlaying = true;
        }
    }
    void Play(const std::string& clipName) {
        for (size_t i = 0; i < m_clips.size(); i++) {
        if (m_clips[i].name != clipName) continue;
            PlayClip(i);
            return;
        }
    }

    // アニメーションの停止
    void Stop() {
        m_isPlaying = false;
        m_currentClipIndex = -1;
        m_currentFrameIndex = 0;
        m_frameTimer = 0.0f;
    }

};

#endif // SPRITE_ANIMATION_COMPONENT_H
