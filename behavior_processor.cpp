//===================================================
// behavior_processor.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//===================================================
#include "behavior_processor.h"
#include "game_object.h"
#include "scene_interface.h"

#include "mi_math.h"

#include "behavior_component.h"

// BehaviorProcessorの初期化
void BehaviorProcessor::Initialize()
{
    m_startList.reserve(MAX_BEHAVIORS);
    m_updateList.reserve(MAX_BEHAVIORS);
}

// BehaviorProcessorの終了処理
void BehaviorProcessor::Finalize()
{

}

// BehaviorProcessorの処理
void BehaviorProcessor::Process(IScene* pScene)
{
    auto behaviorComponents = pScene->GetComponentsByBaseType<BehaviorComponent>();
    if (behaviorComponents.empty()) return;

    for (BehaviorComponent* behavior : behaviorComponents) {
        if (!behavior->GetOwner()->GetActive()) continue;
        if (!behavior->GetEnable()) continue;

        // Start()を呼び出すべきBehaviorComponentをキューに追加
        if (!behavior->m_hasStarted) {
            m_startList.push_back(behavior);
        }

        // Update()を呼び出すべきBehaviorComponentをキューに追加
        m_updateList.push_back(behavior);
    }

    // Start()の呼び出し
    for (BehaviorComponent* behavior : m_startList) {
        behavior->Start();
        behavior->m_hasStarted = true; // Start()が呼び出されたことを記録
    }

    // Update()の呼び出し
    for (BehaviorComponent* behavior : m_updateList) {
        behavior->Update();
    }

    m_startList.clear();
    m_updateList.clear();
}