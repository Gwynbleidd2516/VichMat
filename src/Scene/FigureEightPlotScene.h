#ifndef FIGURE_EIGHT_PLOT_SCENE
#define FIGURE_EIGHT_PLOT_SCENE

#include "PlotScene.h"

class FigureEightPlotScene : public PlotScene
{
public:
    FigureEightPlotScene()
    {
        const double PERIOD = 6.3259;
        this->mName = "Figure eight";
        this->params.h = 0.001;
        this->params.T_end = PERIOD * 10;
        this->params.output_every = 10;
        this->params.realtime = true;
    }

    void init() override
    {
        init_figure_eight(*state);
    }
};

#endif