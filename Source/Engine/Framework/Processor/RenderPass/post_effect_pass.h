//---------------------------------------------------
// post_effect_pass.h
//
// Author: Miu Kitamura
// Date  : 2026/04/29
//---------------------------------------------------
#ifndef POST_EFFECT_PASS_H
#define POST_EFFECT_PASS_H

#include "Engine/Core/pass.h"
#include "Engine/System/Device/direct3d.h"

class PostEffectPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Finalize() override;
    void Process(IScene* pScene) override;

};

#endif // POST_EFFECT_PASS_H
