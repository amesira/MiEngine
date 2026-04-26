//===================================================
// player_dodge_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/04/26
//===================================================
#include "player_dodge_behavior.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Utility/mi_math.h"
#include "Engine/System/Device/mi_fps.h"

#include "Engine/Framework/Component/transform_component.h"
#include "Engine/Framework/Component/rigidbody_component.h"

void PlayerDodgeBehavior::Start()
{
    GameObject* owner = this->GetOwner();
    if (!owner) return;

    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
}

void PlayerDodgeBehavior::Update()
{

}

void PlayerDodgeBehavior::DrawComponentInspector()
{

}

//------------------------------- public

// 回避状態開始処理
void PlayerDodgeBehavior::StartDodge(PlayerContext& context)
{
    // パラメータリセット
    m_isDodging = true;
    m_dodgeTimer = 0.0f;
    m_isJustDodge = false;

    // 無敵状態に設定

}

// プレイヤーの回避状態更新処理
void PlayerDodgeBehavior::UpdateDodge(PlayerContext& context, float deltaTime)
{
    if (!m_isDodging) return;
    float deltaTime = FPS_GetDeltaTime();
    m_dodgeTimer += deltaTime;

    // ジャスト回避の判定
    if (!m_isJustDodge) {
        // ToDo：いろいろ判定条件を書く
        m_isJustDodge = true;
    }

    // 回避終了
    if (m_dodgeTimer >= m_dodgeDuration) {
        m_isDodging = false;
        m_dodgeTimer = 0.0f;
        m_isJustDodge = false;
    }
}