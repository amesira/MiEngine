//---------------------------------------------------
// ui_render_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//---------------------------------------------------
#ifndef UI_RENDER_PASS_H
#define UI_RENDER_PASS_H
#include "Engine/Core/pass.h"
#include <vector>
#include "./Context/draw_command.h"

#include "./Context/collector_image.h"
#include "./Context/collector_slider.h"
#include "./Context/collector_font.h"

class UIRenderPass : public Pass {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    ID3D11Buffer* m_pVertexBuffer = nullptr;

    // 描画コマンドのバッチリスト
    std::vector<DrawBatch2D> m_batches;

    //　描画コマンドの収集クラス
    CollectorImage  m_collectorImage;
    CollectorSlider m_collectorSlider;
    CollectorFont   m_collectorFont;

public:
    void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void    Finalize() override;
    void    Process(IScene* pScene) override;

    void    AddDrawBatch(const DrawBatch2D& batch) { m_batches.push_back(batch); }

private:


};

#endif // UI_RENDER_PASS_H