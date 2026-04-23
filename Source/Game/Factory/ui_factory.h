//---------------------------------------------------
// ui_factory.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/14
//---------------------------------------------------
#ifndef UI_FACTORY_H
#define UI_FACTORY_H
#include <DirectXMath.h>
using namespace DirectX;
#include <string>

class GameObject;
class SceneBase;

namespace UiFactory {
    // UIイメージ生成
    GameObject* CreateUiImage(SceneBase* scene, const std::wstring& texturePath);
    // UIテキスト生成
    GameObject* CreateUiText(SceneBase* scene, const std::u8string& text);
    // UIスライダー生成
    GameObject* CreateUiSlider(SceneBase* scene, const XMFLOAT4& bgColor, const XMFLOAT4& fillColor, float value);
    // UITransformセットアップ
    bool    SetupUiTransform(GameObject* uiElement, const XMFLOAT2& position, const XMFLOAT2& size);

};

#endif // !UI_FACTORY_H