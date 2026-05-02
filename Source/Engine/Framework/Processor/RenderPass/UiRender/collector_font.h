//----------------------------------------------------
// collector_font.h
// 
// Author：Miu Kitamura (from Ushi)
// Date  ：2025/11/18
//----------------------------------------------------
#ifndef RENDERER_FONT_PROCESSOR_H
#define RENDERER_FONT_PROCESSOR_H
#include <string>
#include <vector>
#include <map>
#include <d3d11.h>
#include "External/stb_truetype.h"

#include "Windows.h"
#include "wrl/client.h"
using Microsoft::WRL::ComPtr;

#include "Engine/Framework/Component/text_component.h"
#include "draw_command.h"

class IScene;

class CollectorFont {
private:
	// 1文字の描画情報 (変更なし)
	struct GlyphInfo {
		float x_off, y_off;
		float width, height;
		float x_advance;
		float u0, v0, u1, v1;
	};

	// stb_truetype関連 (変更なし)
	static constexpr float BASE_FONT_SIZE = 64.0f;
	std::vector<unsigned char> m_fontBuffer[(int)TextComponent::Font::MAX];
	stbtt_fontinfo m_fontInfo[(int)TextComponent::Font::MAX];
	std::map<int, GlyphInfo> m_glyphCache[(int)TextComponent::Font::MAX];
	int m_atlasWidth = 1024;
	int m_atlasHeight = 1024;
	int m_atlasCursorX = 0;
	int m_atlasCursorY = 0;
	int m_currentLineHeight = 0;

	// --- ★変更: DirectX11関連のメンバ ---
	ComPtr<ID3D11Texture2D> m_pFontTexture[(int)TextComponent::Font::MAX];
	ComPtr<ID3D11ShaderResourceView> m_pFontSRV[(int)TextComponent::Font::MAX];
	ComPtr<ID3D11Buffer> m_pVertexBuffer;

public:
    void    Initialize();
    void    Finalize();

	void CollectDrawBatches2D(IScene* pScene, std::vector<DrawBatch2D>& outBatches);

private:
	int DecodeUtf8(const char8_t** text_ptr);
	const GlyphInfo* GetGlyph(int fontType, int codepoint);
};

#endif