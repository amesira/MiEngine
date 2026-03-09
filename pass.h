//---------------------------------------------------
// pass.h [Pass基底クラス]
// 
// ・Processorの内部で処理を行う単位。
// ・実際の処理の内容を記述するのはPassの方で、
// Processorは複数のPassを持ち、処理の順番を管理
// 
// Author：Miu Kitamura
// Date  ：2026/03/09
//---------------------------------------------------
#ifndef PASS_H
#define PASS_H

class IScene;

class Pass {
public:
    virtual ~Pass() = default;

    virtual void    Initialize() = 0;
    virtual void    Finalize() = 0;

    virtual void    Process(IScene* pScene) = 0;

};

#endif // PASS_H