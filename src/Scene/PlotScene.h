#ifndef PLOT_SCENE
#define PLOT_SCENE

#include "IScene.h"
#include "imgui.h"
#include "implot.h"   // ImPlot is a 2D library, but ImPlot3D uses its context
#include "implot3d.h" // The 3D extension header
#include "Mmath.h"

#include <iostream>
#include <format>
#include <string>
using namespace std;

class PlotScene : IScene
{
protected:
    SystemState *state;
    ExperimentParams params;
    int mStep;
    string mName;

public:
    PlotScene()
    {
        mStep = 0;
    }

    void setState(int n, Method meth)
    {
        state = new SystemState(n);
        init();
    }

    void step()
    {

        std::ofstream out;
        if (!params.realtime)
        {
            out.open(params.output_file);
            if (!out)
                throw std::runtime_error("Cannot open: " + params.output_file);
            write_csv_header(out, state->N);
        }

        int steps = static_cast<int>(params.T_end / params.h);

        // Лямбда: шаг нужного метода
        auto do_step = [&](SystemState &s)
        {
            switch (params.method)
            {
            case Method::EULER:
                Euler_step(s, params.h);
                break;
            case Method::LEAPFROG:
                Leapfrog_step(s, params.h);
                break;
            case Method::RK4:
                RK4_step(s, params.h);
                break;
            };
        };

        if (mStep <= steps)
        {
            double t = mStep * params.h;

            if (mStep % params.output_every == 0)
            {
                if (!params.realtime)
                {
                    write_csv_row(out, t, *state);
                }
            }

            if (mStep < steps)
                do_step(*state);
            mStep++;
        }
    }

    virtual void display() override
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        // Set window flags for a borderless, full-screen appearance
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_NoBackground; // Optional: transparent background

        ImGui::Begin("Plot", nullptr, window_flags);

        if (ImPlot3D::BeginPlot(mName.c_str(), ImVec2(-1, -1)))
        {
            ImPlot3D::SetupAxes("x", "y", "z");
            step();

            for (int i = 0; i < state->bodies.size(); i++)
            {
                ImPlot3D::PlotScatter(format("Body {}", i + 1).c_str(), &(state->bodies[i]).x, &(state->bodies[i]).y, &(state->bodies[i]).z, 1);
            }

            ImPlot3D::EndPlot();
        }

        ImGui::End();

        ImGui::Render();

        ImGuiIO &io = ImGui::GetIO();

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        // glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glutSwapBuffers();
        glutPostRedisplay();
    }

    ~PlotScene()
    {
        // delete[] mDataX;
        // delete[] mDataY;
        // delete[] mDataZ;
        delete state;
    }

    protected:

    virtual void init() = 0;
};

#endif