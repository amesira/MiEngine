//===================================================
// sprite_animation_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/05/29
//===================================================
#include "sprite_animation_processor.h"

#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/Device/mi_fps.h"
#include "Utility/mi_math.h"

#include "Engine/Framework/Component/sprite_animation_component.h"
#include "Engine/Framework/Component/sprite_renderer_component.h"

void SpriteAnimationProcessor::Initialize()
{

}

void SpriteAnimationProcessor::Finalize()
{

}

void SpriteAnimationProcessor::Process(IScene* pScene)
{
    float deltaTime = FPS_GetDeltaTime();

    // コンポーネントプール取得
    auto* spriteAnimationPool = pScene->GetComponentPool<SpriteAnimationComponent>();
    auto* spriteRendererPool = pScene->GetComponentPool<SpriteRendererComponent>();
    if (!spriteAnimationPool || !spriteRendererPool)return;

    auto& animationList = spriteAnimationPool->GetList();
    for (auto& anim : animationList) {
        if (!anim.GetEnable()) continue;
        SpriteRendererComponent* spriteRenderer = spriteRendererPool->GetByGameObjectID(anim.GetOwner()->GetID());
        if (!spriteRenderer) continue;

        // アニメーションの再生チェック
        if (!anim.m_isPlaying) continue;

        // ===== フレーム処理 =====
        
        // 現在再生中のクリップを取得
        const std::vector<SpriteAnimationComponent::Clip>& clips = anim.m_clips;
        int currentClipIndex = anim.m_clips.empty() ? -1 : anim.m_currentClipIndex;
        if (currentClipIndex < 0 || currentClipIndex >= clips.size()) continue;
        const SpriteAnimationComponent::Clip& currentClip = clips[currentClipIndex];

        // 再生中のフレームを取得
        if (currentClip.frames.empty()) continue;
        int& currentFrameIndex = anim.m_currentFrameIndex;
        if (currentFrameIndex < 0 || currentFrameIndex >= currentClip.frames.size()) {
            currentFrameIndex = 0;
        }
        const SpriteAnimationComponent::Frame& frame = currentClip.frames[currentFrameIndex];
        float& frameTimer = anim.m_frameTimer;

        // フレームタイマーを更新
        frameTimer += deltaTime * currentClip.speed;
        float frameDuration = frame.duration;

        // 次のフレームに進むかチェック
        if (frameTimer >= frameDuration) {
            frameTimer = 0.0f;
            currentFrameIndex++;
            if (currentFrameIndex >= currentClip.frames.size()) {
                if (currentClip.loop) {
                    currentFrameIndex = 0;
                }
                else {
                    // アニメーション終了
                    currentFrameIndex = static_cast<int>(currentClip.frames.size()) - 1;
                    anim.Stop();
                }
            }
        }

        // ===== Rendererへの適用処理 =====
        SpriteAnimationComponent::Frame newFrame = currentClip.frames[currentFrameIndex];
        TextureResource* newFrameTex = newFrame.textureResource;
        XMFLOAT4 uvRect = newFrame.uvRect;
        XMFLOAT4 color = newFrame.color; // 点滅処理とか作れそう

        spriteRenderer->SetTextureResource(newFrameTex);
        spriteRenderer->SetUvRect(uvRect);
        spriteRenderer->SetColor(color);

    }
}