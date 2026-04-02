#ifndef BUTTERFLY_PLOT_SCENE
#define BUTTERFLY_PLOT_SCENE

#include "PlotScene.h"

// Сцена с хореографией "Бабочка I" (3 тела, из каталога Simo 2002).
// Нетривиальная 3-тельная хореография — форма траектории напоминает крылья бабочки.
// Нестабильна, поэтому рекомендуется DP8_ADAPTIVE для наблюдения.
class ButterflyPlotScene : public PlotScene
{
public:
    ButterflyPlotScene()
    {
        this->mName = "Butterfly I (Simo, 3 bodies)";
        this->params.h            = 0.001;
        this->params.T_end        = 60.0;   // примерный период неизвестен точно — берём запас
        this->params.output_every = 10;
        this->params.realtime     = true;
        this->params.tol_rel      = 1e-11;
        this->params.tol_abs      = 1e-13;
    }

    void setState(int /*n*/, Method meth) { PlotScene::setState(3, meth); }
    void init() override { init_butterfly(*state); }
};

// Сцена с хореографией "Мотылёк" (3 тела, из каталога Simo 2002).
// Похожа на бабочку, но с другой формой петель.
class MothPlotScene : public PlotScene
{
public:
    MothPlotScene()
    {
        this->mName = "Moth I (Simo, 3 bodies)";
        this->params.h            = 0.001;
        this->params.T_end        = 60.0;
        this->params.output_every = 10;
        this->params.realtime     = true;
        this->params.tol_rel      = 1e-11;
        this->params.tol_abs      = 1e-13;
    }

    void setState(int /*n*/, Method meth) { PlotScene::setState(3, meth); }
    void init() override { init_moth(*state); }
};

#endif