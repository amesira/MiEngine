//----------------------------------------------------
// factory.h [ゲームオブジェクト工場]
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#ifndef FACTORY_H
#define FACTORY_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;

#include "game_object.h"

namespace Factory {
    
    void CreateBox          (GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color);
    
    void CreateModel(GameObject* obj, const char* modelPath, XMFLOAT3 position, XMFLOAT3 scaling);

    void CreateJointGroup(GameObject* jointGroup, XMFLOAT3 startPosition, XMFLOAT3 endPosition, float interval);
}

#endif