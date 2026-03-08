//===================================================
// processor.h [Processor基底クラス]
// 
// ・Component群を用いて処理を行う処理装置。
// ・当たり判定、物理演算、描画処理など、多数のGameObjectで使いたいかつ、
// 　処理順が重要になるものを作成する。
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "component.h"

class IScene;

class Processor {
public:
    virtual ~Processor() = default;

    virtual void    Initialize() {}
    virtual void    Finalize() {}

    virtual void    Process(IScene* pScene) {}

};

#endif