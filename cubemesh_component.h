//===================================================
// cubemesh_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#ifndef CUBEMESH_COMPONENT_H
#define CUBEMESH_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include "component.h"

class CubemeshComponent : public Component {
private:
    ID3D11ShaderResourceView* m_pTexture = NULL;
    DirectX::XMFLOAT4   m_color = { 1.0f,1.0f,1.0f ,1.0f };

public:
    void    SetTexture(ID3D11ShaderResourceView* tex) { m_pTexture = tex; }
    void    SetColor(DirectX::XMFLOAT4 color) { m_color = color; }

    ID3D11ShaderResourceView*   GetTexture() { return m_pTexture; }
    DirectX::XMFLOAT4           GetColor() { return m_color; }
};

#endif