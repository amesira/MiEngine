//----------------------------------------------------
// factory.h [ゲームオブジェクト工場]
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#ifndef FACTORY_H
#define FACTORY_H

#include "Engine/Device/direct3d.h"
using namespace DirectX;

#include "Engine/Core/game_object.h"

namespace Factory {
    // Box型のGameObjectを生成する関数
    void CreateBox          (GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color);
    // Model型のGameObjectを生成する関数
    void CreateModel(GameObject* obj, const char* modelPath, XMFLOAT3 position, XMFLOAT3 scaling);
    // JointGroup型のGameObjectを生成する関数
    void CreateJointGroup(GameObject* jointGroup, XMFLOAT3 startPosition, XMFLOAT3 endPosition, float interval);

    // Fieldを生成する関数
    void CreateField(GameObject* field, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color);

}

#endif