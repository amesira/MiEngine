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
class ShaderProgramResource;

class CollectorFont {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // 頂点バッファ
	ComPtr<ID3D11Buffer> m_pVertexBuffer;

    // シェーダープログラムリソース
    ShaderProgramResource* m_pFontShader = nullptr;

	// 1文字の描画情報
	struct GlyphInfo {
		float x_off, y_off;
		float width, height;
		float x_advance;
		float u0, v0, u1, v1;
	};

	// フォント情報構造体
	struct FontInfo {
		stbtt_fontinfo fontInfo;
		std::vector<unsigned char> fontBuffer;
		std::map<int, GlyphInfo> glyphCache; // コードポイント -> グリフ情報

        ComPtr<ID3D11Texture2D> fontTexture;
        ComPtr<ID3D11ShaderResourceView> fontSRV;
    };
    FontInfo m_fonts[(int)TextComponent::Font::MAX];

    // ベースフォントサイズ
	static constexpr float BASE_FONT_SIZE = 64.0f;

    // テクスチャアトラスの管理
	int m_atlasWidth = 1024;
	int m_atlasHeight = 1024;
	int m_atlasCursorX = 0;
	int m_atlasCursorY = 0;
	int m_currentLineHeight = 0;

public:
    void    Initialize();
    void    Finalize();

    // シーンからTextComponentを収集して描画バッチを生成
	void CollectDrawBatches2D(IScene* pScene, std::vector<DrawBatch2D>& outBatches);

private:
    // UTF-8デコード関数。text_ptrが指す位置から1文字分のコードポイントを読み取り、text_ptrを次の文字の位置に進める。
	int DecodeUtf8(const char8_t** text_ptr);
    // 指定したフォントタイプとコードポイントに対応するグリフ情報を取得。キャッシュにない場合は新たに生成してキャッシュに保存する。
	const GlyphInfo* GetGlyph(int fontType, int codepoint);
};

#endif