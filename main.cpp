//===================================================
// main.cpp [DirectXを始める]
// 
// Author：Miu Kitamura
// Date  ：2025/05/11
//===================================================
#include <SDKDDKVer.h>      // 利用できる最も上位のWindowsプラットフォームが定義される
#define WIN32_LEAN_AND_MEAN // 32bitアプリには不要な情報を抑止してコンパイル時間を短縮

#include <windows.h>
#include "debug_ostream.h"

#include <algorithm>
#include "direct3d.h"
#include "shader.h"
#include "sprite.h"
#include "keyboard.h"
#include "Audio.h"
#include "mouse.h"

#include "manager.h"

#include "mi_fps.h"

#include "model.h"

//===================================================
// マクロ定義
//===================================================
#define CLASS_NAME "MyWindowClass"
#define WINDOW_CAPTION "ゴルフゲーム"

#define SCREEN_WIDTH  (1280)
#define SCREEN_HEIGHT (720)

#pragma comment(lib,"winmm.lib")

//===================================================
// プロトタイプ宣言
//===================================================
// ウィンドウプロシージャ（コールバック関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//===================================================
// WinMain関数
//===================================================
int APIENTRY WinMain(HINSTANCE hInstance,  // このプログラムを表す情報（識別子？）
    HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmpShow) {

    // 乱数の初期化
    srand(timeGetTime());

    // COMライブラリの初期化
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

    // 変数未使用時の対応（書かなくてもいい）
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmpShow);

    // ウィンドウクラスの登録（ウィンドウの仕様を決めてWindowsにセット）
    WNDCLASS wc;                                        // 構造体を準備
    ZeroMemory(&wc, sizeof(WNDCLASS));                  // 内容を0で初期化
    wc.lpfnWndProc = WndProc;                           // コールバック関数のポインタ
    wc.lpszClassName = CLASS_NAME;                      // 仕様書名
    wc.hInstance = hInstance;                           // アプリケーションの識別子
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);           // カーソルの種類
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);  // ウィンドウの背景色
    RegisterClass(&wc);                                 // 構造体をWindowsへセット

    // ウィンドウサイズの調整
    // クライアント領域（描画領域）のサイズを表す矩形
    RECT window_rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

    // ウィンドウスタイルの設定
    DWORD window_style = WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);
    
    // 指定のクライアント領域＋ウィンドウスタイルの全体サイズを計算
    AdjustWindowRect(&window_rect, window_style, FALSE);
    
    // 矩形の横と縦のサイズを計算
    int window_width = window_rect.right - window_rect.left;
    int window_height = window_rect.bottom - window_rect.top;

    /* ウィンドウの作成 */
    HWND hWnd = CreateWindow(
        CLASS_NAME,         // 作りたいウィンドウ
        WINDOW_CAPTION,     // ウィンドウに表示するタイトル
        window_style, // 標準的な形状のウィンドウ（サイズの変更を禁止）
        CW_USEDEFAULT,
        CW_USEDEFAULT, 
        window_width,       // ウィンドウの幅
        window_height,      // ウィンドウの高さ
        NULL,
        NULL,
        hInstance,          // アプリケーションハンドル
        NULL
    );

    /* 作成したウィンドウを表示 */
    ShowWindow(hWnd, nCmpShow);

    // ウィンドウ内部の更新要求（付けておいた方が安全）
    UpdateWindow(hWnd);

    //----------------------------------------------------
	// 初期化処理
	//----------------------------------------------------
    Direct3D_Initialize(hWnd);
    Keyboard_Initialize();
    Mouse_Initialize(hWnd);

    ID3D11Device* pDevice = Direct3D_GetDevice();
    ID3D11DeviceContext* pContext = Direct3D_GetDeviceContext();

    Shader_Initialize(pDevice, pContext);
    InitializeSprite();

    InitAudio();

    Model_Initialize();

    Manager_Initialize(pDevice, pContext);

    // メッセージループ
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // fps initialize
    FPS_Initialize(hWnd);

    //----------------------------------------------------
	// 終了メッセージがくるまでループする
	//----------------------------------------------------
    do {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            /* 通常はWindowsのメッセージを処理 */
            TranslateMessage(&msg);
            DispatchMessage(&msg);  // WndProcが呼び出される
        }
        else {
            if(FPS_Update()){

                // 更新処理
                Manager_Update();

                // 描画処理
                Manager_Draw();

                keycopy();
                mousecopy();

                // 処理回数更新
                FPS_UpdateFrameCount();
            }
        }
    }while (msg.message != WM_QUIT); // プログラム終了でなければループ

    //----------------------------------------------------
	// 終了処理
	//----------------------------------------------------
    Manager_Finalize();

    UninitAudio();

    Model_Finalize();

    FinalizeSprite();
    Shader_Finalize();

    Direct3D_Finalize();

    //hal::dout << "終了" << std::endl;

    // 終了する
    return (int)msg.wParam;
}

//===================================================
// ウィンドウプロシージャ
// …メッセージループ内で呼び出される
//===================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HGDIOBJ hbrWhite, hbrGray;
    HDC hdc;        // ウィンドウ画面を表す情報（デバイスコンテキスト、入出力先）
    PAINTSTRUCT ps; // ウィンドウ画面の描画関連の情報（大きさなど）

    switch (uMsg) {
    case WM_ACTIVATEAPP:
        Mouse_ProcessMessage(uMsg, wParam, lParam);
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard_ProcessMessage(uMsg, wParam, lParam);
        break;

    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        Mouse_ProcessMessage(uMsg, wParam, lParam);
        break;

    case WM_KEYDOWN: // キーが押された
        if (wParam == VK_ESCAPE) {
            // ウィンドウを閉じたいリクエストをWindowsに送る
            SendMessage(hWnd, WM_CLOSE, 0, 0);
        }
        Keyboard_ProcessMessage(uMsg, wParam, lParam);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        //if (MessageBox(hWnd, "終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
        //    DestroyWindow(hWnd);
        //}
        //else {
        //    // 終了キャンセル
        //    return 0;
        //}
        break;
    case WM_DESTROY:
        PostQuitMessage(0); // 自分のメッセージの0を送る
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}