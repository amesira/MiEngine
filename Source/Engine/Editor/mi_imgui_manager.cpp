//===================================================
// mi_imgui_manager.cpp [imguiマネージャー制御]
// 
// imguiウィンドウの一括管理や描画などを行う。
// 
// Author：Miu Kitamura
// Date  ：2025/07/01
//===================================================
#include "mi_imgui_manager.h"
#include "Engine/System/Device/direct3d.h"

#include "./EditorWindow/imgui_window_interface.h"

// Imguiの初期化
void MiImguiManager::Initialize(HWND hwnd)
{
    // 1. ImGuiコンテキストを作成
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // 2. IO設定
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // 3. プラットフォーム用初期化（Win32）
    ImGui_ImplWin32_Init(hwnd);

    // 4. DirectX11用初期化
    ImGui_ImplDX11_Init(Direct3D_GetDevice(), Direct3D_GetDeviceContext());

    // 5. スタイル設定
    SetupEditorImguiStyle();

}

// Imguiの終了処理
void MiImguiManager::Finalize()
{
    // ImGuiの終了処理
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// フレーム開始の処理
void MiImguiManager::BeginFrame()
{
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();

    ImGui::NewFrame();
}

// フレーム終了の処理
void MiImguiManager::EndFrame()
{
    // フレーム終了（描画コマンドの準備）
    ImGui::Render();

    // レンダリング（ImGui描画実行）
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// -------------------------------- private

// エディタ用のImguiスタイル設定
void MiImguiManager::SetupEditorImguiStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    {
        // Windowの余白
        style.WindowPadding = ImVec2(5, 5);
        // フレーム（タブタイトルなど）の余白
        style.FramePadding = ImVec2(5, 5);
        style.FrameRounding = 3.0f;
    
        style.ItemSpacing = ImVec2(8, 6);
        style.ItemInnerSpacing = ImVec2(6, 4);

    }

    ImVec4* colors = style.Colors;
    {
        // テキスト色
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);

        // ウィンドウ背景色
        colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
  
        // タイトルバー背景色
        colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

        // タブの背景色
        colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    }
}