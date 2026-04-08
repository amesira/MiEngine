//===================================================
// animation_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/06
//===================================================
#include "animation_processor.h"
#include "scene_interface.h"
#include "game_object.h"

#include "mi_fps.h"
#include "mi_math.h"

#include "model_component.h"

void AnimationProcessor::Initialize() 
{
}

void AnimationProcessor::Finalize() 
{
}

void AnimationProcessor::Process(IScene* pScene) 
{
    // コンポーネントプール取得
    auto* modelPool = pScene->GetComponentPool<ModelComponent>();
    if (!modelPool)return;

    static float timer = 0.0f;
    float deltaTime = FPS_GetDeltaTime() * 1.5f;
    timer += deltaTime;

    // アニメーションの更新
    auto& modelComponents = modelPool->GetList();
    for (auto& modelComp : modelComponents) {
        if (!modelComp.GetEnable()) continue;

        // モデルリソースのアニメーションを再生
        ModelResource* modelRes = modelComp.GetModelResource();
        if (!modelRes) continue;

        // ここではモデルリソースの最初のアニメーションクリップを再生する例
        AnimationClip* clip = modelRes->animationClips.empty() ? nullptr : &modelRes->animationClips[0];
        if (!clip) continue;

        SkeletonPose pose = modelComp.GetSkeletonPose();

        // ここでは単純に時間経過でアニメーションをループ再生する例
        float animTime = fmod(timer, clip->duration);
        for (auto& channel : clip->channels) {

            // ボーンのインデックスを取得
            unsigned int boneIndex = channel.boneIndex;
            if (boneIndex >= modelComp.GetSkeletonPose().boneTransforms.size()) continue;

            // 位置、回転、スケールのキーフレームを線形補間して計算
            XMFLOAT4 position = pose.defaultPositions[boneIndex];
            XMFLOAT4 rotation = pose.defaultRotations[boneIndex];
            XMFLOAT4 scaling = pose.defaultScales[boneIndex];

            // 位置のキーフレーム補間
            if (!channel.positionKeyframes.empty()) {
                position = SamplingKeyframes(channel.positionKeyframes, animTime);
            }
            // 回転のキーフレーム補間
            if (!channel.rotationKeyframes.empty()) {
                rotation = SamplingKeyframes(channel.rotationKeyframes, animTime);
            }
            // スケールのキーフレーム補間
            if (!channel.scalingKeyframes.empty()) {
                scaling = SamplingKeyframes(channel.scalingKeyframes, animTime);
            }

            // ローカル変換行列の計算
            XMMATRIX localTransform = XMMatrixIdentity();
            {
                XMMATRIX scalingMat = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
                XMMATRIX rotationMat = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
                XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
                localTransform = scalingMat * rotationMat * translationMat;

            }
            pose.localTransforms[boneIndex] = localTransform;
        }

        // ルートボーンから順にグローバル変換行列を計算
        std::function<void(unsigned int, const XMMATRIX&)> calculateGlobalTransform = 
            [&](unsigned int index, const XMMATRIX& parentTransform) {

            XMMATRIX localTransform = pose.localTransforms[index];
            XMMATRIX globalTransform = localTransform * parentTransform;

            // グローバル変換行列を保存
            pose.globalTransforms[index] = globalTransform;

            // 子ノードを再帰的に処理
            for (unsigned int i : modelRes->bones[index].childIndices) {
                calculateGlobalTransform(i, globalTransform);
            }
        };
        calculateGlobalTransform(modelRes->rootBoneIndex, modelRes->rootParentCorrection);

        // 最終的なボーン変換行列の計算（グローバル変換行列とオフセット行列を掛け合わせる）
        for (unsigned int i = 0; i < pose.boneTransforms.size(); i++) {
            pose.boneTransforms[i] = modelRes->bones[i].offsetMatrix * pose.globalTransforms[i];
        }

        modelComp.SetSkeletonPose(pose);
    }


}

// キーフレームの線形補間
XMFLOAT4 AnimationProcessor::SamplingKeyframes(const std::vector<AnimationClip::Keyframe>& keyframes, float time)
{
    if (keyframes.empty()) return XMFLOAT4(0, 0, 0, 1); // キーフレームがない場合はデフォルト値を返す
    XMFLOAT4 result = keyframes[0].keyValue; // 最初のキーフレームの値を初期値とする

    // 前後のキーフレームを見つける
    size_t prevIndex = 0;
    size_t nextIndex = 0;

    // （キーフレームは時間順にソートされている前提）
    for (size_t i = 0; i < keyframes.size(); i++) {
        if (keyframes[i].time > time) {
            nextIndex = i;
            break;
        }
        prevIndex = i;
    }
    if (nextIndex == 0) nextIndex = keyframes.size() - 1; // 最後のキーフレームをループ

    float t = (time - keyframes[prevIndex].time) / (keyframes[nextIndex].time - keyframes[prevIndex].time);
    t = MiMath::Clamp(t, 0.0f, 1.0f); // tを0～1の範囲にクランプ

    result = MiMath::Lerp(keyframes[prevIndex].keyValue, keyframes[nextIndex].keyValue, t);

    return result;
}
