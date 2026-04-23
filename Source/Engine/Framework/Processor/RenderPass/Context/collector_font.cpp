//===================================================
// collector_font.cpp
// 
// ・TrueTypeフォントをレンダリングするプロセッサー
// 
// Author：Miu Kitamura (from Ushi)
// Date  ：2025/11/18
//===================================================
#define NOMINMAX
#include "collector_font.h"
#include "Utility/debug_ostream.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include "Engine/System/Device/direct3d.h"

#include "Engine/Core/scene_interface.h"
#include "Engine/Core/game_object.h"

#include "Engine/Framework/Component/rect_transform_component.h"

#include "Utility/mi_math.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static const char* fontPath[(int)TextComponent::Font::MAX] = {
	"asset/Font/PixelMplus12-Regular.ttf",
};

void CollectorFont::Initialize()
{
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

	//----------------------------------------------------
	// フォントデータの読み込みとテクスチャアトラス作成
	//----------------------------------------------------
	for (int i = 0; i < (int)TextComponent::Font::MAX; i++) {
		// ファイルパスから読み込み
		std::ifstream file(fontPath[i], std::ios::binary);
		if (!file)return;
		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		m_fontBuffer[i].resize(size);
		file.read((char*)m_fontBuffer[i].data(), size);

		if (!stbtt_InitFont(&m_fontInfo[i], m_fontBuffer[i].data(), 0)) {
			return;
		}

		// 1. テクスチャアトラスの作成
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = m_atlasWidth;
		texDesc.Height = m_atlasHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8_UNORM; // 8bitの赤チャンネルのみ
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		g_pDevice->CreateTexture2D(&texDesc, nullptr, m_pFontTexture[i].GetAddressOf());

		// 2. シェーダーリソースビューの作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		g_pDevice->CreateShaderResourceView(m_pFontTexture[i].Get(), &srvDesc, m_pFontSRV[i].GetAddressOf());
	}
}

void CollectorFont::Finalize()
{
	
}

// シーンからTextComponentを収集して描画バッチを生成
void CollectorFont::CollectDrawBatches2D(IScene* pScene, std::vector<DrawBatch2D>& outBatches)
{
	auto* textPool = pScene->GetComponentPool<TextComponent>();
	auto* rectTransformPool = pScene->GetComponentPool<RectTransformComponent>();
    if (textPool == nullptr || rectTransformPool == nullptr)return;

    auto& textList = textPool->GetList();

	for(TextComponent& t : textList) {
		TextComponent* text = &t;
		RectTransformComponent* rect = rectTransformPool->GetByGameObjectID(text->GetOwner()->GetID());

        // コンポーネントが無効ならスキップ
        if (!rect) continue;
		if (!rect->GetOwner()->GetActive()) continue;
		if (!rect->GetEnable()) continue;

        int fontType = (int)text->GetFontType();
        if (!m_pFontSRV[fontType]) continue;

		// 描画スケール計算
		float render_scale = (float)text->GetFontSize() / BASE_FONT_SIZE;
        const char8_t* current_char = text->GetText().c_str();

        
		// 描画コマンドに追加
		DrawBatch2D batch;
		batch.orderInLayer = rect->GetPosition().z;
		batch.texture = m_pFontSRV[fontType].Get();
        batch.shaderType = DrawBatch2D::ShaderType::Font;

		DrawCommand2DInstance instance;
		instance.angleZ = rect->GetRotation().z;
		instance.color = text->GetColor();

		// 各文字の出力
		XMFLOAT3 rectPos = rect->GetPosition();

        XMFLOAT3 pivotPos = { 0.0f, 0.0f, 0.0f };
		{
			// Textを中央揃えにする場合、開始位置を調整
			float xOffset = 0.0f;
			if (text->IsCenter()) {
				while (*current_char != '\0')
				{
					int codepoint = DecodeUtf8(&current_char);
					if (codepoint == 0) break;

					const GlyphInfo* glyph = GetGlyph(fontType, codepoint);
					if (!glyph) continue;

					xOffset += (glyph->x_advance * render_scale) * 0.5f;
				}
			}
			pivotPos.x -= xOffset;
		}

		current_char = text->GetText().c_str();
		while (*current_char != '\0') {
			int codepoint = DecodeUtf8(&current_char);
			if (codepoint == 0) break;

			const GlyphInfo* glyph = GetGlyph(fontType, codepoint);
			if (!glyph) continue;

            // ローカルな頂点データを計算
			float x0 = pivotPos.x + (glyph->x_off * render_scale);
			float y0 = pivotPos.y + (glyph->y_off * render_scale); // ★Y方向を反転
			float x1 = x0 + (glyph->width * render_scale);
			float y1 = y0 + (glyph->height * render_scale); // ★Y方向を反転

			// インスタンス更新
            XMFLOAT2 localPos = { (x0 + x1) / 2.0f, (y0 + y1) / 2.0f };
			{
				// ピボット位置からのオフセットを回転させて加算
				float cosAngle = cosf(instance.angleZ);
				float sinAngle = sinf(instance.angleZ);
				XMFLOAT3 rotatedOffset = {
                    localPos.x* cosAngle - localPos.y * sinAngle,
                    localPos.x* sinAngle + localPos.y * cosAngle,
					0.0f
				};
				instance.position = {
					rectPos.x + rotatedOffset.x,
					rectPos.y + rotatedOffset.y,
                };
			}
			
			instance.size = {
				x1 - x0,
				y1 - y0
            };
			instance.uvRect = {
				glyph->u0, glyph->v0, glyph->u1 - glyph->u0, glyph->v1 - glyph->v0
			};

			// 文字送り (変更なし)
			pivotPos.x += (glyph->x_advance * render_scale);

			// インスタンス追加
			batch.instances.push_back(instance);
		}

		// バッチ追加
		outBatches.push_back(batch);
    }
}

// UTF-8デコード関数
int CollectorFont::DecodeUtf8(const char8_t** text_ptr) 
{
	const unsigned char* s = (const unsigned char*)*text_ptr;
	unsigned int c = *s;
	unsigned int c2, c3, c4;

	if (c < 0x80) { // 1-byte sequence (ASCII)
		*text_ptr += 1;
		return c;
	}
	else if ((c & 0xE0) == 0xC0) { // 2-byte sequence
		if (s[1] != '\0') {
			c2 = s[1];
			if ((c2 & 0xC0) == 0x80) {
				*text_ptr += 2;
				return ((c & 0x1F) << 6) | (c2 & 0x3F);
			}
		}
	}
	else if ((c & 0xF0) == 0xE0) { // 3-byte sequence
		if (s[1] != '\0' && s[2] != '\0') {
			c2 = s[1]; c3 = s[2];
			if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80) {
				*text_ptr += 3;
				return ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
			}
		}
	}
	else if ((c & 0xF8) == 0xF0) { // 4-byte sequence
		if (s[1] != '\0' && s[2] != '\0' && s[3] != '\0') {
			c2 = s[1]; c3 = s[2]; c4 = s[3];
			if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80 && (c4 & 0xC0) == 0x80) {
				*text_ptr += 4;
				return ((c & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
			}
		}
	}

	// 不正な文字の場合は1バイト進めて置換文字を返す
	*text_ptr += 1;
	return 0xFFFD; // Replacement Character
}

// グリフ情報を取得する関数
const CollectorFont::GlyphInfo* CollectorFont::GetGlyph(int fontType, int codepoint)
{
	auto it = m_glyphCache[fontType].find(codepoint);
	if (it != m_glyphCache[fontType].end()) {
		return &it->second;
	}

	// --- 2. キャッシュになければ、ベースサイズで新しいグリフを生成 ---

	// ★ ベースサイズに対応するスケールを計算
	float base_scale = stbtt_ScaleForPixelHeight(&m_fontInfo[fontType], BASE_FONT_SIZE);

	int glyphIndex = stbtt_FindGlyphIndex(&m_fontInfo[fontType], codepoint);
	if (glyphIndex == 0) return nullptr;

	// グリフのビットマップ矩形をベーススケールで取得
	int x0, y0, x1, y1;
	stbtt_GetGlyphBitmapBox(&m_fontInfo[fontType], glyphIndex, base_scale, base_scale, &x0, &y0, &x1, &y1);

	int glyphWidth = x1 - x0;
	int glyphHeight = y1 - y0;

	if (glyphWidth <= 0 || glyphHeight <= 0)
	{
		// テクスチャへの書き込みは行わない
		glyphHeight = BASE_FONT_SIZE;
		glyphWidth = BASE_FONT_SIZE / 2;
	}

	// アトラスのカーソル位置を更新 (改行処理)
	if (m_atlasCursorX + glyphWidth > m_atlasWidth) {
		m_atlasCursorX = 0;
		m_atlasCursorY += m_currentLineHeight;
		m_currentLineHeight = 0;
	}
	// アトラスが一杯になったらエラー
	if (m_atlasCursorY + glyphHeight > m_atlasHeight) {
		hal::dout << "Error: Font atlas is full!" << std::endl;
		return nullptr;
	}

	// 3. グリフを一時的なビットマップにレンダリング
	std::vector<unsigned char> glyphBitmap(glyphWidth * glyphHeight);
	stbtt_MakeGlyphBitmap(&m_fontInfo[fontType], glyphBitmap.data(), glyphWidth, glyphHeight, glyphWidth, base_scale, base_scale, glyphIndex);

	// --- ★変更: テクスチャアトラスへの転送を UpdateSubresource で行う ---
	D3D11_BOX destBox;
	destBox.left = m_atlasCursorX;
	destBox.right = m_atlasCursorX + glyphWidth;
	destBox.top = m_atlasCursorY;
	destBox.bottom = m_atlasCursorY + glyphHeight;
	destBox.front = 0;
	destBox.back = 1;

	g_pContext->UpdateSubresource(
		m_pFontTexture[fontType].Get(), // 更新先テクスチャ
		0,                    // Mipレベル
		&destBox,             // 更新先の矩形領域
		glyphBitmap.data(),   // 送信するデータ
		glyphWidth,           // 1行のバイト数
		0                     // 3Dテクスチャではないので0
	);

	// 5. 新しいグリフの情報を計算してキャッシュに保存
	GlyphInfo newGlyph;
	// 描画オフセットと矩形サイズ (ベースサイズ基準)
	newGlyph.x_off = static_cast<float>(x0);
	newGlyph.y_off = static_cast<float>(y0);
	newGlyph.width = static_cast<float>(glyphWidth);
	newGlyph.height = static_cast<float>(glyphHeight);
	// UV座標
	newGlyph.u0 = static_cast<float>(m_atlasCursorX) / m_atlasWidth;
	newGlyph.v0 = static_cast<float>(m_atlasCursorY) / m_atlasHeight;
	newGlyph.u1 = static_cast<float>(m_atlasCursorX + glyphWidth) / m_atlasWidth;
	newGlyph.v1 = static_cast<float>(m_atlasCursorY + glyphHeight) / m_atlasHeight;

	// 文字送り幅をベーススケールで取得
	int advanceWidth;
	stbtt_GetGlyphHMetrics(&m_fontInfo[fontType], glyphIndex, &advanceWidth, nullptr);
	newGlyph.x_advance = static_cast<float>(advanceWidth) * base_scale;

	// アトラスカーソル位置を更新
	m_atlasCursorX += glyphWidth + 1; // 1ピクセルのパディング
	m_currentLineHeight = std::max(m_currentLineHeight, glyphHeight + 1);

	// 作成したグリフをキャッシュして、その参照を返す
	auto [inserted_it, success] = m_glyphCache[fontType].emplace(codepoint, newGlyph);
	return &inserted_it->second;
}