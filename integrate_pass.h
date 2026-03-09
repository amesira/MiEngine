//----------------------------------------------------
// integrate_pass.h [物理演算パス]
// 
// ・重力、摩擦などをvelocity（速度）に設定し、位置への適用を行う
// ・物理演算群の1番目にあたる。
// 
// Author：Miu Kitamura
// Date  ：2026/03/10
//----------------------------------------------------
#ifndef INTEGRATE_PASS_H
#define INTEGRATE_PASS_H
#include "pass.h"

class IntegratePass : public Pass {
public:
    void    Initialize()override;
    void    Finalize()override;

    void    Process(IScene* pScene)override;

};

#endif // INTEGRATE_PASS_H