#ifndef PLOT_SCENE
#define PLOT_SCENE

#include "IScene.h"
#include "imgui.h"
#include "implot.h"
#include "implot3d.h"
#include "../Mmath.h"
#include <sstream>
#include <iostream>
#include <string>
using namespace std;

// Базовый класс для всех сцен с 3D-анимацией.
// Хранит состояние системы тел, параметры метода и умеет делать шаги симуляции.
// Конкретные сцены (восьмёрка, многоугольник и др.) наследуются отсюда и реализуют init().
class PlotScene : IScene
{
protected:
    SystemState *state;      // текущее состояние всей системы тел
    ExperimentParams params; // шаг h, метод, T_end и т.д.
    int mStep;               // счётчик шагов (чтобы не выйти за T_end)
    string mName;            // название — показывается в заголовке ImPlot3D
    bool initialized;        // нельзя рисовать до вызова setState()
    double mCurrentH;        // текущий шаг (меняется при адаптивном методе)

public:
    PlotScene() : state(nullptr), mStep(0), initialized(false), mCurrentH(0.001)
    {
        params.method = Method::RK4;
    }

    // Создаём систему из n тел, устанавливаем метод, вызываем init() дочернего класса.
    // Метод Method::DP8_ADAPTIVE автоматически подбирает шаг — h лишь стартовое значение.
    void setState(int n, Method meth)
    {
        if (state) delete state;
        state = new SystemState(n);
        params.method = meth;
        mCurrentH = params.h;  // стартовый шаг для адаптивного метода
        std::cout << "setState: n=" << n << ", method=" << static_cast<int>(meth) << std::endl;
        init();
        initialized = true;
        mStep = 0;
    }

    // Один шаг симуляции выбранным методом.
    // При DP8_ADAPTIVE шаг mCurrentH меняется каждую итерацию (возвращается из функции).
    // Вызывается из display() каждый кадр — так и получается анимация.
    void step()
    {
        if (!state || !initialized) return;
        int steps = static_cast<int>(params.T_end / params.h);

        if (mStep <= steps)
        {
            if (mStep < steps)
            {
                switch (params.method)
                {
                case Method::EULER:
                    Euler_step(*state, params.h);
                    break;
                case Method::LEAPFROG:
                    Leapfrog_step(*state, params.h);
                    break;
                case Method::RK4:
                    RK4_step(*state, params.h);
                    break;
                case Method::DP8:
                    DP8_step(*state, params.h);
                    break;
                case Method::DP8_ADAPTIVE:
                    // Возвращает предложенный шаг для следующей итерации
                    mCurrentH = DP8_adaptive_step(*state, mCurrentH,
                                                  params.tol_rel, params.tol_abs);
                    break;
                }
            }
            mStep++;
        }
    }

    // Вызывается каждый кадр из glutDisplayFunc / glutIdleFunc.
    // Порядок: ImGui-фрейм → шаг физики → рисуем тела в 3D → swap buffers.
    virtual void display() override
    {
        if (!state || !initialized)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glutSwapBuffers();
            return;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();

        // Окно на весь экран, без декораций
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_NoBackground;

        ImGui::Begin("Plot", nullptr, window_flags);

        // ImPlot3D рисует интерактивный 3D-график; -1,-1 = занять всё доступное место
        if (ImPlot3D::BeginPlot(mName.c_str(), ImVec2(-1, -1)))
        {
            ImPlot3D::SetupAxes("x", "y", "z");

            // Делаем один шаг симуляции прямо внутри кадра рендеринга
            step();

            // Рисуем каждое тело как отдельную точку-рассеяние с подписью "Body i"
            for (int i = 0; i < (int)state->bodies.size(); i++)
            {
                std::stringstream ss;
                ss << "Body " << i + 1;
                ImPlot3D::PlotScatter(ss.str().c_str(),
                                      &state->bodies[i].x,
                                      &state->bodies[i].y,
                                      &state->bodies[i].z, 1);
            }

            ImPlot3D::EndPlot();
        }

        ImGui::End();
        ImGui::Render();

        ImGuiIO &io = ImGui::GetIO();
        ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.15f, 1.00f);
        glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glutSwapBuffers();
        glutPostRedisplay();  // просим GLUT перерисовать как можно скорее (анимация)
    }

    virtual ~PlotScene() { delete state; }

protected:
    // Чисто виртуальный: наследник обязан задать начальные условия через init_*()
    virtual void init() = 0;
};

#endif
