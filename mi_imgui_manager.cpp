//===================================================
// mi_imgui_manager.cpp [imguiマネージャー制御]
// 
// imguiウィンドウの一括管理や描画などを行う。
// 
// Author：Miu Kitamura
// Date  ：2025/07/01
//===================================================
#include "mi_imgui_manager.h"
#include "direct3d.h"

// Imguiの初期化
void MiImguiManager::Initialize(HWND hwnd)
{
    // 1. ImGuiコンテキストを作成
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // 2. IO設定など（省略可）
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // 3. プラットフォーム用初期化（Win32）
    ImGui_ImplWin32_Init(hwnd);

    // 4. DirectX11用初期化（ここが重要！）
    ImGui_ImplDX11_Init(Direct3D_GetDevice(), Direct3D_GetDeviceContext());

    // 5. スタイル設定など
    ImGui::StyleColorsDark(); // 任意

    // 6. ウィンドウリストの初期化（必要に応じて）
    m_imguiWindowList.clear();
}

// Imguiの終了処理
void MiImguiManager::Finalize()
{
    // ImGuiの終了処理
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // ウィンドウリストの解放
    m_imguiWindowList.clear();
}

// imguiウィンドウ一括描画処理
void MiImguiManager::RenderProcess()
{
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();

    ImGui::NewFrame();

    for (auto& window : m_imguiWindowList) {
        window->Draw();
    }

    // フレーム終了（描画コマンドの準備）
    ImGui::Render();

    // レンダリング（ImGui描画実行）
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
