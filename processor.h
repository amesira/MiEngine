//---------------------------------------------------
// processor.h [Processor基底クラス]
// 
// ・Component群を用いて処理を行う処理装置。
// ・当たり判定、物理演算、描画処理など、多数のGameObjectで使いたいかつ、
// 　処理順が重要になるものを作成する。
// 
// 2026/03/09
// ・上位の処理単位として扱うように変更。
// ・Processorは内部で複数のPassを持ち、Processor::Process()内では
// Pass::Process()を呼び出す形にする。
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//---------------------------------------------------
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