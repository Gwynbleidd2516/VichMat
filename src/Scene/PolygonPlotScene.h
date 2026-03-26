#ifndef POLYGON_PLOT_SCENE
#define POLYGON_PLOT_SCENE

#include "PlotScene.h"

class PolygonPlotScene : public PlotScene
{
public:
    PolygonPlotScene()
    {
        this->mName = "Polygon";
        this->params.h = 0.001;
        this->params.T_end = 30.0;
        this->params.output_every = 10;
        this->params.output_file = "polygon5.csv";
    }

    void init() override
    {
        init_polygon(*state);
    }
};

#endif