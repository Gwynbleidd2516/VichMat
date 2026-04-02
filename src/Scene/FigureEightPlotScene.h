#ifndef FIGURE_EIGHT_PLOT_SCENE
#define FIGURE_EIGHT_PLOT_SCENE

#include "PlotScene.h"

// Сцена с фигурой восьмёркой (3 тела, Chenciner & Montgomery 2000).
// Три тела гоняются по одной замкнутой траектории в форме восьмёрки.
// Период T ≈ 6.3259; моделируем 10 полных периодов.
// Хороший тест для сравнения методов: с Эйлером восьмёрка разваливается быстро,
// с DP8_ADAPTIVE держится очень долго.
class FigureEightPlotScene : public PlotScene
{
public:
    FigureEightPlotScene()
    {
        const double PERIOD = 6.3259;
        this->mName = "Figure eight";
        this->params.h = 0.001;           // стартовый шаг (для адаптивного — только начало)
        this->params.T_end = PERIOD * 10;
        this->params.output_every = 10;
        this->params.realtime = true;
        this->params.tol_rel  = 1e-10;    // допуск для DP8_ADAPTIVE
        this->params.tol_abs  = 1e-12;
    }

    void init() override { init_figure_eight(*state); }
};

#endif