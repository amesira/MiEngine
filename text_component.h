//===================================================
// text_component.h
// 
// Author：Miu Kitamura
// Date  ：2025/11/18
//===================================================
#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

// directX
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"

#include <string>

#include "component.h"

class TextComponent : public Component {
public:
    enum class Font {
        Default,
        // 追加のフォントタイプがあればここに列挙する。

        MAX,
    };

private:
    std::u8string       m_text = u8"";
    DirectX::XMFLOAT4   m_color = { 1.0f,1.0f,1.0f,1.0f };

    Font                m_fontType = Font::Default;
    int                 m_fontSize = 32;

    bool                m_center = false; // 中央揃えフラグ

public:
    void    SetText(const std::u8string& text) { m_text = text; }
    void    SetColor(DirectX::XMFLOAT4 color) { m_color = color; }
    void    SetFontType(Font fontType) { m_fontType = fontType; }
    void    SetFontSize(int fontSize) { m_fontSize = fontSize; }
    void    SetCenter(bool center) { m_center = center; }

    const std::u8string& GetText()const { return m_text; }
    DirectX::XMFLOAT4   GetColor()const { return m_color; }
    Font    GetFontType()const { return m_fontType; }
    int     GetFontSize()const { return m_fontSize; }
    bool    IsCenter()const { return m_center; }

};

#endif