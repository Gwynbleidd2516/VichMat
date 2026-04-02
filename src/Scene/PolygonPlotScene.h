#ifndef POLYGON_PLOT_SCENE
#define POLYGON_PLOT_SCENE

#include "PlotScene.h"

// Сцена с N телами на окружности (правильный N-угольник).
// N задаётся снаружи через setState(N, method) — от 3 до 20 тел.
// Это "тривиальная" хореография: все тела вращаются по окружности
// с правильно подобранной угловой скоростью omega.
class PolygonPlotScene : public PlotScene
{
public:
    PolygonPlotScene()
    {
        this->mName = "Polygon";
        this->params.h            = 0.001;
        this->params.T_end        = 30.0;
        this->params.output_every = 10;
        this->params.output_file  = "polygon.csv";
        this->params.tol_rel      = 1e-10;
        this->params.tol_abs      = 1e-12;
    }

    void init() override { init_polygon(*state); }
};

#endif
