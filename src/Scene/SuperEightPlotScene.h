#ifndef SUPER_EIGHT_PLOT_SCENE
#define SUPER_EIGHT_PLOT_SCENE

#include "PlotScene.h"

// Сцена с супер-восьмёркой Жервера (4 тела, Gerver 2000).
// Жервер нашёл её через неделю после объявления восьмёрки Ченсинера-Монтгомери.
// Четыре тела гоняются по кривой с тремя петлями.
// Орбита НЕСТАБИЛЬНА — даже с RK4 разваливается за несколько периодов.
// Для наблюдения хореографии рекомендуется метод DP8 или DP8_ADAPTIVE.
// Период T ≈ 28.67 (G=1, m=1).
class SuperEightPlotScene : public PlotScene
{
public:
    SuperEightPlotScene()
    {
        this->mName = "Super-eight (Gerver, 4 bodies)";
        this->params.h            = 0.001;
        this->params.T_end        = 28.67 * 3;  // три периода
        this->params.output_every = 10;
        this->params.realtime     = true;
        this->params.tol_rel      = 1e-12;  // нужна высокая точность — орбита нестабильна
        this->params.tol_abs      = 1e-14;
    }

    // setState для супер-восьмёрки всегда использует 4 тела,
    // независимо от того, что передал пользователь
    void setState(int /*n*/, Method meth)
    {
        PlotScene::setState(4, meth);
    }

    void init() override { init_super_eight(*state); }
};

#endif
