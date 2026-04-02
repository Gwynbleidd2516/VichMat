#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glut.h"
#include "backends/imgui_impl_opengl3.h"

#include "Scene/PolygonPlotScene.h"
#include "Scene/FigureEightPlotScene.h"
#include "Scene/SuperEightPlotScene.h"
#include "Scene/ButterflyPlotScene.h"

#include "Mmath.h"

static PlotScene *scene = nullptr;

// Статус последнего сравнения — показывается пользователю под кнопкой
static std::string g_compare_status = "";

// Вспомогательная функция: создаёт начальное состояние для выбранной фигуры
static SystemState make_initial_state(int figure, int N)
{
    int actualN = N;
    if (figure == 0 || figure == 3 || figure == 4) actualN = 3;
    if (figure == 2) actualN = 4;

    SystemState s(actualN);
    switch (figure)
    {
    case 0: init_figure_eight(s); break;
    case 1: init_polygon(s);      break;
    case 2: init_super_eight(s);  break;
    case 3: init_butterfly(s);    break;
    case 4: init_moth(s);         break;
    }
    return s;
}

void mainLoop()
{
    if (!scene)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();
        ImGuiIO &io = ImGui::GetIO();

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings;
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::Begin("Welcome", nullptr, window_flags);

        static int figure = 0;
        static int N      = 5;
        static int method = 2; // RK4 по умолчанию

        float windowWidth = ImGui::GetWindowWidth();

        const char* title = "Laboratory work on computational mathematics No. 1";
        ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(title).x) * 0.5f);
        ImGui::Text("%s", title);
        ImGui::Spacing(); ImGui::Spacing();

        const char* authors = "Authors: Vyaznikov N.K, Rositsky V.";
        ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(authors).x) * 0.5f);
        ImGui::Text("%s", authors);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // --- Выбор фигуры ---
        const char *figure_items[] = {
            "Figure Eight (3 bodies)",
            "Polygon (N bodies)",
            "Super-Eight / Gerver (4 bodies)",
            "Butterfly I (3 bodies)",
            "Moth I (3 bodies)"
        };
        ImGui::Combo("Select figure", &figure, figure_items, IM_ARRAYSIZE(figure_items));

        if (figure == 1)
        {
            ImGui::InputInt("Number of bodies", &N);
            if (N < 3)  N = 3;
            if (N > 20) N = 20;
        }
        else
        {
            int fixedN[] = {3, 0, 4, 3, 3};
            ImGui::TextDisabled("  Number of bodies: %d (fixed)", fixedN[figure]);
        }

        ImGui::Spacing();

        // --- Выбор метода ---
        const char *method_items[] = {
            "Euler (order 1)",
            "Leapfrog (order 2, symplectic)",
            "RK4 (order 4)",
            "DP8 (order 8, fixed step)",
            "DP8 Adaptive (order 8, auto step)"
        };
        ImGui::Combo("Select method", &method, method_items, IM_ARRAYSIZE(method_items));
        if (method == 4)
            ImGui::TextDisabled("  Tolerances: 1e-10 (rel), 1e-12 (abs)");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // --- Кнопка Start ---
        float buttonWidth = 120.0f;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
        if (ImGui::Button("Start", ImVec2(buttonWidth, 40.0f)))
        {
            switch (figure)
            {
            case 0: scene = new FigureEightPlotScene(); break;
            case 1: scene = new PolygonPlotScene();     break;
            case 2: scene = new SuperEightPlotScene();  break;
            case 3: scene = new ButterflyPlotScene();   break;
            case 4: scene = new MothPlotScene();        break;
            }
            if (scene)
            {
                Method meth;
                switch (method)
                {
                case 0: meth = Method::EULER;        break;
                case 1: meth = Method::LEAPFROG;     break;
                case 2: meth = Method::RK4;          break;
                case 3: meth = Method::DP8;          break;
                case 4: meth = Method::DP8_ADAPTIVE; break;
                default: meth = Method::RK4;         break;
                }
                scene->setState(N, meth);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ----------------------------------------------------------------
        //  Блок сравнения методов
        // ----------------------------------------------------------------
        //
        //  Нажатие кнопки запускает compare_methods() — она прогоняет
        //  Euler, Leapfrog, RK4 и DP8 с одинаковых начальных условий
        //  и записывает в CSV:
        //    - t               — время
        //    - E_Euler, ...    — энергия каждого метода (мера его точности)
        //    - dist_X_vs_Y     — суммарное расстояние между телами методов X и Y
        //
        //  Это синхронная операция (программа «зависнет» на несколько секунд),
        //  потому что считается в том же потоке. Для типичного T_end ≈ 60 это
        //  занимает 2-5 секунд с h=0.001.
        //
        //  Файл пишется рядом с exe. Открыть в Excel/Python/Matlab для анализа.

        ImGui::TextDisabled("-- Method comparison --");
        ImGui::Spacing();

        // Параметры сравнения
        static float compare_h      = 0.001f;
        static float compare_T_end  = 30.0f;
        static int   compare_every  = 50;    // записываем каждые 50 шагов (~600 строк)

        ImGui::PushItemWidth(200);
        ImGui::InputFloat("Step h",         &compare_h,     0.0001f, 0.001f, "%.4f");
        ImGui::InputFloat("T end",          &compare_T_end, 1.0f,    10.0f,  "%.1f");
        ImGui::InputInt  ("Record every N steps", &compare_every);
        ImGui::PopItemWidth();

        if (compare_h     < 1e-5f)  compare_h     = 1e-5f;
        if (compare_h     > 0.1f)   compare_h     = 0.1f;
        if (compare_T_end < 1.0f)   compare_T_end = 1.0f;
        if (compare_every < 1)      compare_every = 1;

        ImGui::Spacing();

        float cmpBtnWidth = 200.0f;
        ImGui::SetCursorPosX((windowWidth - cmpBtnWidth) * 0.5f);
        if (ImGui::Button("Run comparison", ImVec2(cmpBtnWidth, 35.0f)))
        {
            // Строим начальное состояние для выбранной фигуры
            SystemState initial = make_initial_state(figure, N);

            // Сравниваем только фиксированные методы (DP8_ADAPTIVE не подходит —
            // у него переменный шаг, поэтому состояния в разные моменты времени
            // несопоставимы с остальными; его лучше считать эталоном отдельно)
            std::vector<Method> cmp_methods = {
                Method::EULER,
                Method::LEAPFROG,
                Method::RK4,
                Method::DP8
            };
            std::vector<std::string> cmp_names = {
                "Euler", "Leapfrog", "RK4", "DP8"
            };

            // Имя файла включает название фигуры и шаг
            std::string fname = "comparison_"
                + std::string(figure_items[figure]).substr(0, figure_items[figure][0]=='F' ? 6 : 4)
                + "_h" + std::to_string((int)(compare_h * 10000))
                + ".csv";
            // Убираем пробелы из имени файла
            for (char &c : fname) if (c == ' ') c = '_';

            g_compare_status = "Running... (may take a few seconds)";

            try
            {
                CompareResult res = compare_methods(
                    initial,
                    cmp_methods,
                    cmp_names,
                    (double)compare_h,
                    (double)compare_T_end,
                    compare_every,
                    fname
                );

                // Формируем строку статуса с RMSD относительно DP8 (эталон)
                // DP8 — последний в списке (индекс 3), сравниваем с ним остальные
                g_compare_status = "Done! Saved: " + fname + "\n"
                    "RMSD vs DP8:  "
                    "Euler="   + std::to_string(res.rmsd[0][3]).substr(0,8) + "  "
                    "Leapfrog=" + std::to_string(res.rmsd[1][3]).substr(0,8) + "  "
                    "RK4="     + std::to_string(res.rmsd[2][3]).substr(0,8);
            }
            catch (const std::exception &ex)
            {
                g_compare_status = std::string("Error: ") + ex.what();
            }
        }

        // Показываем статус последнего запуска
        if (!g_compare_status.empty())
        {
            ImGui::Spacing();
            ImGui::TextWrapped("%s", g_compare_status.c_str());
        }

        ImGui::End();
        ImGui::Render();

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glutSwapBuffers();
        glutPostRedisplay();
    }
    else
    {
        scene->display();
    }
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

int main(int argc, char **argv)
{
    std::cout << "Program started" << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextVersion(3, 3);
    glutInitWindowSize(1280, 720);

    GLint window = glutCreateWindow("VichMat lb1");
    if (window == 0) { std::cerr << "Failed to create window!\n"; return 1; }

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) { std::cerr << "GLEW Error: " << glewGetErrorString(err) << "\n"; return 1; }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImPlot3D::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui_ImplGLUT_InstallFuncs();

    glutDisplayFunc(mainLoop);
    glutReshapeFunc(reshape);
    glutIdleFunc(mainLoop);

    std::cout << "Entering main loop..." << std::endl;
    glutMainLoop();

    delete scene;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImPlot::DestroyContext();
    ImPlot3D::DestroyContext();
    ImGui::DestroyContext();
    return 0;
}