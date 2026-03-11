//===================================================
// sprite.cpp [スプライト制御]
// 
// 2Dスプライトの初期化、描画、破棄などの管理を行う。
// ※DirectX 11 を使用している。
// 
// Author：Miu Kitamura
// Date  ：2025/06/17
//===================================================
#include "sprite.h"

#include "debug_ostream.h"
#include "shader.h"


//===================================================
// グローバル変数（Direct3Dデバイス／バッファ関連）
//===================================================

// 使用可能な最大頂点数
static constexpr int NUM_VERTEX = 4;
static constexpr int MAX_INSTANCE = 1024;

// 頂点バッファ：スプライト描画
static ID3D11Buffer* g_pVertexBuffer = nullptr;
static ID3D11Buffer* g_pInstanceBuffer = nullptr;

static InstanceData* g_InstanceData = nullptr;
static int g_InstanceCount = 0;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

//===================================================
// スプライト初期化
//===================================================
void InitializeSprite() {

	// デバイスの取得
	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetDeviceContext();

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX; // 格納する最大頂点数
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

	// インスタンスバッファ生成
    bd.ByteWidth = sizeof(InstanceData) * MAX_INSTANCE; // 格納する最大インスタンス数
    g_pDevice->CreateBuffer(&bd, NULL, &g_pInstanceBuffer);
    g_InstanceCount = 0;
}

void LoadTexture(ID3D11ShaderResourceView** texture, const wchar_t* fileName) {
	TexMetadata metadata;
	ScratchImage image;

	HRESULT hr = LoadFromWICFile(fileName, WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) {
		hal::dout << "failed load texture" << std::endl;
		return;
	}
	hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, texture);
	if (FAILED(hr)) {
		hal::dout << "failed create resource" << std::endl;
		return;
	}
	assert(*texture); // 読み込み失敗時にダイアログを表示
}

//===================================================
// スプライト終了
//===================================================
void FinalizeSprite() {
	g_pVertexBuffer->Release();
}

//===================================================
// スプライト描画
//===================================================
void DrawSprite(XMFLOAT4 color, XMFLOAT4 uvRect, XMFLOAT3 normal)
{
	//----------------------------------------------------
	// 頂点バッファをロック
	//----------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得 memcpyとかで後から詰めるやり方もある
	Vertex* v = (Vertex*)msr.pData;

	// スプライト1つ当たりの比率
	float w = uvRect.z;
	float h = uvRect.w;

	// テクスチャの左上座標を計算
	DirectX::XMFLOAT2 uv;
	uv.x = uvRect.x;
	uv.y = uvRect.y;

	//----------------------------------------------------
	// スプライトの頂点設定
	//----------------------------------------------------
	XMFLOAT2 halfSize = { 0.5f, 0.5f };
	v[0].position = { -halfSize.x,-halfSize.y,0.0f };
	v[0].color = color;
	v[0].texCoord = { uv.x,uv.y };

	v[1].position = { halfSize.x,-halfSize.y,0.0f };
	v[1].color = color;
	v[1].texCoord = { uv.x + w,uv.y };

	v[2].position = { -halfSize.x,halfSize.y,0.0f };
	v[2].color = color;
	v[2].texCoord = { uv.x ,uv.y + h };

	v[3].position = { halfSize.x,halfSize.y,0.0f };
	v[3].color = color;
	v[3].texCoord = { uv.x + w,uv.y + h };

    // 法線の設定
	for (int i = 0; i < 4; i++) {
		v[i].normal = normal;
    }

	// ロック解除 → 描画準備
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定 トライアングルストリップ
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//----------------------------------------------------
	// 描画命令
	//----------------------------------------------------
	g_pContext->Draw(4, 0); // 表示に使用する頂点数を指定
}

void SetTexture(ID3D11ShaderResourceView* texture)
{
	// シェーダーリソース（テクスチャ）設定
    g_pContext->PSSetShaderResources(0, 1, &texture);
}

void PrepareDrawInstance()
{
    //-------------------------
    // 頂点バッファの設定
	//-------------------------
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得 memcpyとかで後から詰めるやり方もある
	Vertex* v = (Vertex*)msr.pData;
    v[0].position = { -0.5f,-0.5f,0.0f };
    v[1].position = { 0.5f,-0.5f,0.0f };
    v[2].position = { -0.5f,0.5f,0.0f };
    v[3].position = { 0.5f,0.5f,0.0f };

    v[0].texCoord = { 0.0f,0.0f };
    v[1].texCoord = { 1.0f,0.0f };
    v[2].texCoord = { 0.0f,1.0f };
    v[3].texCoord = { 1.0f,1.0f };

    for (int i = 0; i < 4; i++) {
		v[i].color = { 1.0f,1.0f,1.0f,1.0f };
		v[i].normal = { 0.0f,0.0f,-1.0f };
    }

	// 頂点バッファのロック解除
    g_pContext->Unmap(g_pVertexBuffer, 0);

    //-------------------------
	// インスタンスバッファの設定
    //-------------------------
    g_pContext->Map(g_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    g_InstanceData = (InstanceData*)msr.pData;

    // インスタンスカウント初期化
    g_InstanceCount = 0;
}

void AddInstanceData(const XMMATRIX& world, const XMFLOAT4& color, const XMFLOAT4& uvRect)
{
	if (g_InstanceCount >= MAX_INSTANCE) return;
	InstanceData& instance = g_InstanceData[g_InstanceCount];

	// ワールド行列をセット
	instance.world0 = XMFLOAT4(world.r[0].m128_f32[0], world.r[0].m128_f32[1], world.r[0].m128_f32[2], world.r[0].m128_f32[3]);
	instance.world1 = XMFLOAT4(world.r[1].m128_f32[0], world.r[1].m128_f32[1], world.r[1].m128_f32[2], world.r[1].m128_f32[3]);
	instance.world2 = XMFLOAT4(world.r[2].m128_f32[0], world.r[2].m128_f32[1], world.r[2].m128_f32[2], world.r[2].m128_f32[3]);
	instance.world3 = XMFLOAT4(world.r[3].m128_f32[0], world.r[3].m128_f32[1], world.r[3].m128_f32[2], world.r[3].m128_f32[3]);
	
	// カラーをセット
	instance.color = color;
	
	// UV矩形をセット
	instance.uvRect = uvRect;

	// インスタンスカウントをインクリメント
    g_InstanceCount++;
}

void DrawInstance()
{
	g_pContext->Unmap(g_pInstanceBuffer, 0);

    // 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インスタンスバッファを描画パイプラインに設定
	stride = sizeof(InstanceData);
	offset = 0;
	g_pContext->IASetVertexBuffers(1, 1, &g_pInstanceBuffer, &stride, &offset);

	// プリミティブトポロジ設定 トライアングルストリップ
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 描画命令
	g_pContext->DrawInstanced(4, g_InstanceCount, 0, 0);
	g_InstanceCount = -1;
}

void ClearInstanceData()
{
	// インスタンスバッファのデフォルト設定
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	g_InstanceData = (InstanceData*)msr.pData;
	g_InstanceData[0] = InstanceData();

	g_pContext->Unmap(g_pInstanceBuffer, 0);

	// インスタンスバッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(1, 1, &g_pInstanceBuffer, &stride, &offset);
}