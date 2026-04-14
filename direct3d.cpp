//===================================================
// direct3d.cpp [Direct3Dの初期化関連]
// 
// Author：Miu Kitamura
// Date  ：2025/05/11
//===================================================
#include <d3d11.h>
#include "direct3d.h"
#include "debug_ostream.h"

// ComPtr用のスマートポインタ
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#pragma comment(lib,"d3d11.lib")
// #pragma comment(lib,"dxgi.lib")

// 各種インタフェース
static ID3D11Device*        g_pDevice = nullptr;
static ID3D11DeviceContext* g_pDeviceContext = nullptr;
static IDXGISwapChain*      g_pSwapChain = nullptr;

// バックバッファ関連
static ID3D11RenderTargetView*      g_pRenderTargetView = nullptr;
static ID3D11Texture2D*             g_pDepthStencilBuffer = nullptr;
static ID3D11DepthStencilView*      g_pDepthStencilView = nullptr;
static D3D11_TEXTURE2D_DESC         g_BackBufferDecs{};
static D3D11_VIEWPORT               g_Viewport{};

static bool configureBackBuffer();  // バックバッファの設定・生成
static void releaseBackBuffer();    // バックバッファの解散

// ブレンドステート関連
static float bFactor[4] = { 0.0f,0.0f,0.0f,0.0f };
static ID3D11BlendState* bState[BLENDSTATE_MAX];
static ID3D11DepthStencilState* g_pDepthState[DEPTHSTATE_MAX];

//===================================================
// Direct3D初期化処理
//===================================================
bool Direct3D_Initialize(HWND hWnd)
{
    /* デバイス、スワップチェーン、コンテキスト生成 */
    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swap_chain_desc.OutputWindow = hWnd;

    UINT device_flags = 0;

#if defined(DEBUG)||defined(_DEBUG)
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        device_flags,
        levels,
        ARRAYSIZE(levels),
        D3D11_SDK_VERSION,
        &swap_chain_desc,
        &g_pSwapChain,
        &g_pDevice,
        &feature_level,
        &g_pDeviceContext);

    if (FAILED(hr)) {
        MessageBox(hWnd, "Direct3Dの初期化に失敗しました", "エラー", MB_OK);
        return false;
    }

    if (!configureBackBuffer()) {
        MessageBox(hWnd, "バックバッファの設定に失敗しました", "エラー", MB_OK);
        return false;
    }

    //----------------------------------------------------
	// サンプラーステート設定
	//----------------------------------------------------
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // フィルター
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // 横の座標範囲外は画像繰り返し
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // 縦の座標範囲外は画像繰り返し
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    ID3D11SamplerState* samplerState = NULL;
    g_pDevice->CreateSamplerState(&samplerDesc, &samplerState);

    // サンプラーをシェーダーへセット
    g_pDeviceContext->PSSetSamplers(0, 1, &samplerState);

    //----------------------------------------------------
	// ブレンドステート設定
	//----------------------------------------------------
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    {
        // ブレンド無効
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        g_pDevice->CreateBlendState(&blendDesc, &bState[BLENDSTATE_NONE]);

        // αブレンド
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        g_pDevice->CreateBlendState(&blendDesc, &bState[BLENDSTATE_ALFA]);

        // 加算合成
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        g_pDevice->CreateBlendState(&blendDesc, &bState[BLENDSTATE_ADD]);

        // 減算合成
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
        g_pDevice->CreateBlendState(&blendDesc, &bState[BLENDSTATE_SUB]);
    }
    SetBlendState(BLENDSTATE_ALFA); // デフォルト設定（アルファ）

    //----------------------------------------------------
	// 深度ステンシル設定
	//----------------------------------------------------
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    {
        // 深度有効ステート
        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = FALSE;
        g_pDevice->CreateDepthStencilState(&depthStencilDesc, &g_pDepthState[DEPTHSTATE_ENABLE]);

        // 深度無効ステート
        depthStencilDesc.DepthEnable = FALSE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        g_pDevice->CreateDepthStencilState(&depthStencilDesc, &g_pDepthState[DEPTHSTATE_DISABLE]);

        // 深度書き込み無効ステート
        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        g_pDevice->CreateDepthStencilState(&depthStencilDesc, &g_pDepthState[DEPTHSTATE_NOWRITE]);
    }
    g_pDeviceContext->OMSetDepthStencilState(g_pDepthState[DEPTHSTATE_ENABLE], NULL);

    return true;
}

//===================================================
// Direct3D終了処理
//===================================================
void Direct3D_Finalize()
{
    releaseBackBuffer();

    if (g_pSwapChain) {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }

    if (g_pDeviceContext) {
        g_pDeviceContext->Release();
        g_pDeviceContext = nullptr;
    }

    if (g_pDevice) {
        g_pDevice->Release();
        g_pDevice = nullptr;
    }
}

// バックバッファのクリアとレンダーターゲット、デプスステンシルの設定
void Direct3D_Clear()
{
    /* バックバッファクリア */
    float clear_color[4] = { 0.2f,0.4f,0.8f,1.0f }; // クリア色設定
    g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);
    g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    /* レンダーターゲットビューとデプスステンシルビューの設定 */
    g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    Direct3D_ResetViewport();
}

// スワップチェーンの表示
void Direct3D_Present()
{
    g_pSwapChain->Present(1, 0);
}

// ビューポートの設定
void Direct3D_ResetViewport()
{
    g_pDeviceContext->RSSetViewports(1, &g_Viewport);
}

ID3D11Device* Direct3D_GetDevice() {
    return g_pDevice;
}

ID3D11DeviceContext* Direct3D_GetDeviceContext() {
    return g_pDeviceContext;
}

unsigned int Direct3D_GetBackBufferWidth() {
    return g_BackBufferDecs.Width;
}

unsigned int Direct3D_GetBackBufferHeight() {
    return g_BackBufferDecs.Height;
}

// バックバッファ関連のリソースを生成・設定する関数
bool configureBackBuffer()
{
    HRESULT hr;

    ID3D11Texture2D* back_buffer_pointer = nullptr;

    // バックバッファの取得
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_pointer);

    if (FAILED(hr)) {
        hal::dout << "バックバッファの取得に失敗しました" << std::endl;
        return false;
    }

    // バックバッファのレンダーターゲットビューの生成
    hr = g_pDevice->CreateRenderTargetView(back_buffer_pointer, nullptr, &g_pRenderTargetView);

    if (FAILED(hr)) {
        back_buffer_pointer->Release();
        hal::dout << "バックバッファのレンダーターゲットビューの生成に失敗しました" << std::endl;
        return false;
    }

    // バックバッファの状態（情報）を取得
    back_buffer_pointer->GetDesc(&g_BackBufferDecs);

    back_buffer_pointer->Release(); // バックバッファのポインタは不要なので開放

    // デプスステンシルバッファの生成
    D3D11_TEXTURE2D_DESC depth_stencil_desc{};
    depth_stencil_desc.Width = g_BackBufferDecs.Width;
    depth_stencil_desc.Height = g_BackBufferDecs.Height;
    depth_stencil_desc.MipLevels = 1;
    depth_stencil_desc.ArraySize = 1;
    depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_desc.SampleDesc.Count = 1;
    depth_stencil_desc.SampleDesc.Quality = 0;
    depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_desc.CPUAccessFlags = 0;
    depth_stencil_desc.MiscFlags = 0;
    hr = g_pDevice->CreateTexture2D(&depth_stencil_desc, nullptr, &g_pDepthStencilBuffer);

    if (FAILED(hr)) {
        hal::dout << "デプスステンシルバッファの生成に失敗しました" << std::endl;
        return false;
    }

    // デプスステンシルビューの作成
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
    depth_stencil_view_desc.Format = depth_stencil_desc.Format;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;
    depth_stencil_view_desc.Flags = 0;
    hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilBuffer, &depth_stencil_view_desc,
        &g_pDepthStencilView);

    if (FAILED(hr)) {
        hal::dout << "デプスステンシルビューの生成に失敗しました" << std::endl;
        return false;
    }

    /* ビューポートの設定 */
    g_Viewport.TopLeftX = 0.0f;
    g_Viewport.TopLeftY = 0.0f;
    g_Viewport.Width = static_cast<FLOAT>(g_BackBufferDecs.Width);
    g_Viewport.Height = static_cast<FLOAT>(g_BackBufferDecs.Height);
    g_Viewport.MinDepth = 0.0f;
    g_Viewport.MaxDepth = 1.0f;
    g_pDeviceContext->RSSetViewports(1, &g_Viewport);

    return true;
}

// バックバッファ関連のリソースを解放する関数
void releaseBackBuffer()
{
    if (g_pRenderTargetView) {
        g_pRenderTargetView->Release();
        g_pRenderTargetView = nullptr;
    }

    if (g_pDepthStencilBuffer) {
        g_pDepthStencilBuffer->Release();
        g_pDepthStencilBuffer = nullptr;
    }

    if (g_pDepthStencilView) {
        g_pDepthStencilView->Release();
        g_pDepthStencilView = nullptr;
    }
}

// ブレンドステートの切り替え関数
void SetBlendState(BLENDSTATE blend)
{
    g_pDeviceContext->OMSetBlendState(bState[blend], bFactor, 0xffffff);
}

// デプスステートの切り替え関数
void SetDepthState(DEPTHSTATE depth)
{
    g_pDeviceContext->OMSetDepthStencilState(g_pDepthState[depth], NULL);
}

// シーンテクスチャの内容をスナップショット先へコピーし、SRVを生成する関数
void Direct3D_CreateSnapshotSceneSRV(ID3D11ShaderResourceView** snapshotSrv, ID3D11Texture2D** fromTex)
{
    ID3D11Texture2D* snapshot = nullptr;

    if (*snapshotSrv == nullptr) {
        // g_SceneTextureと同じ設定で、スナップショット先の空テクスチャを生成
        D3D11_TEXTURE2D_DESC desc = {};
        (*fromTex)->GetDesc(&desc);
        g_pDevice->CreateTexture2D(&desc, nullptr, &snapshot);

        // スナップショット先からSRVを生成
        g_pDevice->CreateShaderResourceView(snapshot, nullptr, snapshotSrv);
    }
    else {
        // SRVからリソースを取得し、スナップショット先のテクスチャを得る
        ID3D11Resource* resource = nullptr;
        (*snapshotSrv)->GetResource(&resource);

        resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&snapshot);
        resource->Release();
    }

    // シーンテクスチャの内容をスナップショット先へコピー
    g_pDeviceContext->CopyResource(snapshot, *fromTex);

    snapshot->Release();
}

void Direct3D_ClearSceneTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
    if (rtv) {
        float clear_color[4] = { 0.2f,0.4f,0.8f,1.0f }; // クリア色設定
        g_pDeviceContext->ClearRenderTargetView(rtv, clear_color);
    }
    if (dsv) {
        g_pDeviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
}

// シーンのレンダーターゲットを切り替える関数
void Direct3D_SetSceneTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
    // シーンバッファをレンダーターゲットに設定
    g_pDeviceContext->OMSetRenderTargets(1, &rtv, dsv);
}

// シーンテクスチャの生成と関連ビューの作成
void Direct3D_CreateColorBuffer(ID3D11Texture2D** tex, ID3D11RenderTargetView** rtv, ID3D11ShaderResourceView** srv)
{
    if (!tex && !srv && !rtv) return;

    HRESULT hr;

    ID3D11Texture2D* outTex = nullptr;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = g_BackBufferDecs.Width;
    desc.Height = g_BackBufferDecs.Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    hr = g_pDevice->CreateTexture2D(&desc, nullptr, &outTex);
    if (FAILED(hr) || !(outTex)) {
        hal::dout << "シーンテクスチャの生成に失敗しました" << std::endl;
        return;
    }

    if (srv){
        hr = g_pDevice->CreateShaderResourceView(outTex, nullptr, srv);
        if (FAILED(hr) || !(*srv)) {
            hal::dout << "SRVの生成に失敗しました" << std::endl;
            outTex->Release();
            outTex = nullptr;
            return;
        }
    }

    if (rtv) {
        hr = g_pDevice->CreateRenderTargetView(outTex, nullptr, rtv);
        if (FAILED(hr) || !(*rtv)) {
            hal::dout << "RTVの生成に失敗しました" << std::endl;
            if (srv) {
                (*srv)->Release();
                *srv = nullptr;
            }
            outTex->Release();
            outTex = nullptr;
            return;
        }
    }

    if (tex) {
        *tex = outTex;
    }
     else {
        outTex->Release();
    }
}

// シーンテクスチャの深度ステンシルバッファとビューの生成
void Direct3D_CreateDepthBuffer(ID3D11Texture2D** tex, ID3D11DepthStencilView** dsv, ID3D11ShaderResourceView** srv)
{
    if (!tex || !dsv || !srv) return;
    *tex = nullptr;
    *dsv = nullptr;
    *srv = nullptr;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = g_BackBufferDecs.Width;
    desc.Height = g_BackBufferDecs.Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    HRESULT hr = g_pDevice->CreateTexture2D(&desc, nullptr, tex);
    if (FAILED(hr) || !(*tex)) {
        hal::dout << "シーン用デプスステンシルバッファの生成に失敗しました" << std::endl;
        return;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    dsvDesc.Flags = 0;
    hr = g_pDevice->CreateDepthStencilView(*tex, &dsvDesc, dsv);
    if (FAILED(hr) || !(*dsv)) {
        hal::dout << "シーン用デプスステンシルビューの生成に失敗しました" << std::endl;
        (*tex)->Release();
        *tex = nullptr;
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = g_pDevice->CreateShaderResourceView(*tex, &srvDesc, srv);
    if (FAILED(hr) || !(*srv)) {
        hal::dout << "シーン用デプスステンシルSRVの生成に失敗しました" << std::endl;
        (*dsv)->Release();
        *dsv = nullptr;
        (*tex)->Release();
        *tex = nullptr;
        return;
    }
}
