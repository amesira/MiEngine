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

// レンダリング先シーンテクスチャ関連
static ID3D11Texture2D*             g_pSceneTexture = nullptr;
static ID3D11RenderTargetView*      g_pSceneRtv = nullptr;
static ID3D11ShaderResourceView*    g_pSceneSrv = nullptr;

static bool configureSceneTexture();  // シーンテクスチャの設定・生成
static void releaseSceneTexture();    // シーンテクスチャの解散

// ブレンドステート関連
static float bFactor[4] = { 0.0f,0.0f,0.0f,0.0f };
static ID3D11BlendState* bState[BLENDSTATE_MAX];
static ID3D11DepthStencilState* g_pDepthState[DEPTHSTATE_MAX];
// 行列置き場
static DirectX::XMMATRIX g_viewMatrix = DirectX::XMMatrixIdentity();
static DirectX::XMMATRIX g_projectionMatrix = DirectX::XMMatrixIdentity();

// カメラ位置
static DirectX::XMFLOAT3 g_cameraForward;
static DirectX::XMFLOAT3 g_cameraRight;

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

    if (!configureSceneTexture()) {
        MessageBox(hWnd, "シーンテクスチャの設定に失敗しました", "エラー", MB_OK);
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
    releaseSceneTexture();

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

static float g_clearColor[4] = { 0.2f,0.4f,0.8f,1.0f };

void Direct3D_BeginScene(float r, float g, float b)
{
    // シーンバッファのクリア
    

    //float clear_color[4] = { 0.1f,0.7f,1.0f,1.0f }; // クリア色設定
    g_pDeviceContext->ClearRenderTargetView(g_pSceneRtv, g_clearColor);
    g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // シーンバッファをレンダーターゲットに設定
    g_pDeviceContext->OMSetRenderTargets(1, &g_pSceneRtv, g_pDepthStencilView);

    Direct3D_ResetViewport();
}

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

void Direct3D_Present()
{
    /* スワップチェーンの表示 */
    g_pSwapChain->Present(1, 0);
}

void Direct3D_ResetViewport()
{
    // ビューポートの設定
    g_pDeviceContext->RSSetViewports(1, &g_Viewport);
}

//===================================================
// ゲッター
//===================================================
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

//===================================================
// バックバッファ処理
//===================================================
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

bool configureSceneTexture()
{
    HRESULT hr;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = g_BackBufferDecs.Width;
    texDesc.Height = g_BackBufferDecs.Height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBAの色情報
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D* sceneTexture = nullptr;
    hr = g_pDevice->CreateTexture2D(&texDesc, nullptr, &sceneTexture);
    if (FAILED(hr)) {
        hal::dout << "シーン描画用のテクスチャ生成に失敗しました" << std::endl;
        return false;
    }

    // RTV作成
    hr = g_pDevice->CreateRenderTargetView(sceneTexture, nullptr, &g_pSceneRtv);
    if (FAILED(hr)) {
        sceneTexture->Release();
        hal::dout << "シーン描画用のレンダーターゲットビューの生成に失敗しました" << std::endl;
        return false;
    }

    // SRV作成
    hr = g_pDevice->CreateShaderResourceView(sceneTexture, nullptr, &g_pSceneSrv);
    sceneTexture->Release();
    if (FAILED(hr)) {
        hal::dout << "シーン描画用のシェーダーリソースビューの生成に失敗しました" << std::endl;
        return false;
    }

    return true;
}

void releaseSceneTexture()
{
    if(g_pSceneRtv) {
        g_pSceneRtv->Release();
        g_pSceneRtv = nullptr;
    }
    if(g_pSceneSrv) {
        g_pSceneSrv->Release();
        g_pSceneSrv = nullptr;
    }
    if(g_pSceneTexture) {
        g_pSceneTexture->Release();
        g_pSceneTexture = nullptr;
    }
}

//===================================================
// ブレンドステート設定
//===================================================
void SetBlendState(BLENDSTATE blend)
{
    g_pDeviceContext->OMSetBlendState(bState[blend], bFactor, 0xffffff);
}

void SetDepthState(DEPTHSTATE depth)
{
    g_pDeviceContext->OMSetDepthStencilState(g_pDepthState[depth], NULL);
}

//===================================================
// シーンのスナップショット用SRVを作成
//===================================================
void Direct3D_CreateSnapshotSceneSRV(ID3D11ShaderResourceView** ppSrv)
{
    if (*ppSrv == nullptr) {
        
    }
    else {
        (*ppSrv)->Release();
        *ppSrv = nullptr;
    }

    // SceneSRVの元テクスチャを取得
    ID3D11Texture2D* sceneTex = nullptr;
    g_pSceneSrv->GetResource((ID3D11Resource**)&sceneTex);

    // SceneSRVと同じ設定で、スナップショット先の空テクスチャを生成
    ID3D11Texture2D* snapshot = nullptr;
    D3D11_TEXTURE2D_DESC desc = {};
    sceneTex->GetDesc(&desc);
    {
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    }
    g_pDevice->CreateTexture2D(&desc, nullptr, &snapshot);

    // シーンテクスチャの内容をスナップショット先へコピー
    g_pDeviceContext->CopyResource(snapshot, sceneTex);

    // スナップショット先からSRVを生成
    g_pDevice->CreateShaderResourceView(snapshot, nullptr, ppSrv);

    sceneTex->Release();
    snapshot->Release();
}

DirectX::XMMATRIX& Direct3D_GetViewMatrix()
{
    return g_viewMatrix;
}

DirectX::XMMATRIX& Direct3D_GetProjectionMatrix()
{
    return g_projectionMatrix;
}

void Direct3D_SetViewMatrix(const DirectX::XMMATRIX& matrix)
{
	g_viewMatrix = matrix;
}

void Direct3D_SetProjectionMatrix(const DirectX::XMMATRIX& matrix)
{
	g_projectionMatrix = matrix;
}

void Direct3D_SetCameraInfo(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& atPosition)
{
    DirectX::XMFLOAT3 forward = {
        atPosition.x - position.x,
        0.0f,
        atPosition.z - position.z
    };
    g_cameraForward = forward;
    g_cameraRight = {
        forward.z,
        0.0f,
        -forward.x
    };
}

DirectX::XMFLOAT3 Direct3D_GetCameraForward()
{
    return g_cameraForward;
}

DirectX::XMFLOAT3 Direct3D_GetCameraRight()
{
    return g_cameraRight;
}
