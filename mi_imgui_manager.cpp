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

    SetupEditorImguiStyle();

    // 3. プラットフォーム用初期化（Win32）
    ImGui_ImplWin32_Init(hwnd);

    // 4. DirectX11用初期化（ここが重要！）
    ImGui_ImplDX11_Init(Direct3D_GetDevice(), Direct3D_GetDeviceContext());

    // 5. スタイル設定など

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

void MiImguiManager::SetupEditorImguiStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(5, 5);
    style.FramePadding = ImVec2(5, 5);
    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.22f, 0.24f, 0.29f, 1.00f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.50f, 0.12f, 0.26f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.22f, 0.35f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.28f, 0.10f, 0.13f, 1.00f);
}