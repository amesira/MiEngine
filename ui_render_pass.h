//---------------------------------------------------
// ui_render_pass.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/11
//---------------------------------------------------
#ifndef UI_RENDER_PASS_H
#define UI_RENDER_PASS_H
#include "pass.h"
#include <vector>
#include "draw_command.h"

#include "collector_image.h"
#include "collector_slider.h"
#include "collector_font.h"

class UIRenderPass : public Pass {
private:
    std::vector<DrawBatch2D> m_batches;

    //　描画コマンドの収集クラス
    CollectorImage  m_collectorImage;
    CollectorSlider m_collectorSlider;
    CollectorFont   m_collectorFont;

public:
    void    Initialize() override;
    void    Finalize() override;
    void    Process(IScene* pScene) override;

    void    AddDrawBatch(const DrawBatch2D& batch) { m_batches.push_back(batch); }

private:


};

#endif // UI_RENDER_PASS_H