//---------------------------------------------------
// behavior_processor.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef BEHAVIOR_PROCESSOR_H
#define BEHAVIOR_PROCESSOR_H
#include "Engine/Core/processor.h"
#include <vector>

class BehaviorComponent;

class BehaviorProcessor : public Processor {
private:
    static const int MAX_BEHAVIORS = 1024;

    // Start()を呼び出すべきBehaviorComponentのリスト
    std::vector<BehaviorComponent*> m_startList;

    // Update()を呼び出すべきBehaviorComponentのリスト
    std::vector<BehaviorComponent*> m_updateList;

public:
    void    Initialize() override;
    void    Finalize() override;
    void    Process(IScene* pScene) override;

};

#endif // BEHAVIOR_PROCESSOR_H