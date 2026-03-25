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
    void CreateCamera    (GameObject*, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 atPosition);
    void CreateDirectionalLight(GameObject*, DirectX::XMFLOAT4 direction, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 ambient);
    void CreatePointLight(GameObject* obj, XMFLOAT4 diffuse, float range);

    void CreateBox          (GameObject* cube, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT4 color);
    void CreatePlayer(GameObject* player, DirectX::XMFLOAT3 position);

    void CreateUiImage(GameObject* uiImage, XMFLOAT2 position, XMFLOAT2 size, const wchar_t* texturePath);
    void CreateUiText(GameObject* uiText, XMFLOAT2 position, XMFLOAT2 size, const std::u8string text);
    void CreateUiSlider(GameObject* uiSlider, XMFLOAT2 position, XMFLOAT2 size, float value);

    void CreateModel(GameObject* obj, const char* modelPath, XMFLOAT3 position, XMFLOAT3 scaling);

    void CreateJointGroup(GameObject* jointGroup, XMFLOAT3 startPosition, XMFLOAT3 endPosition, float interval);
}

#endif