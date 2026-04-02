#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glut.h"
#include "backends/imgui_impl_opengl3.h"

#include "Scene/PolygonPlotScene.h"
#include "Scene/FigureEightPlotScene.h"
#include "Scene/SuperEightPlotScene.h"    // новая сцена: супер-восьмёрка Жервера
#include "Scene/ButterflyPlotScene.h"     // новые сцены: бабочка и мотылёк

#include "Mmath.h"

// Текущая активная сцена; nullptr = показываем стартовое меню
static PlotScene *scene = nullptr;

// Главный цикл — вызывается GLUT каждый кадр (и при перерисовке, и в простое).
// Два режима: стартовое меню (scene == nullptr) и анимация (scene != nullptr).
void mainLoop()
{
    if (!scene)
    {
        // ---- Стартовый экран ImGui ----
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();
        ImGuiIO &io = ImGui::GetIO();

        // Окно на весь экран, без рамок и кнопок
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings;
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::Begin("Welcome", nullptr, window_flags);

        static int figure = 0;  // 0=восьмёрка, 1=многоугольник, 2=супер-восьмёрка, 3=бабочка, 4=мотылёк
        static int N      = 5;  // кол-во тел (актуально только для многоугольника)
        static int method = 0;  // 0=Euler, 1=Leapfrog, 2=RK4, 3=DP8, 4=DP8_Adaptive

        // Центрируем заголовок и авторов через SetCursorPosX
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

        // Выбор хореографии
        const char *figure_items[] = {
            "Figure Eight (3 bodies)",       // восьмёрка — устойчивая
            "Polygon (N bodies)",             // правильный N-угольник — тривиальная
            "Super-Eight / Gerver (4 bodies)",// нетривиальная, 4 тела
            "Butterfly I (3 bodies)",         // из каталога Simo 2002
            "Moth I (3 bodies)"               // из каталога Simo 2002
        };
        ImGui::Combo("Select figure", &figure, figure_items, IM_ARRAYSIZE(figure_items));

        // Поле "N тел" — актуально только для многоугольника
        if (figure == 1)
        {
            ImGui::InputInt("Number of bodies", &N);
            if (N < 3)  N = 3;
            if (N > 20) N = 20;
        }

        ImGui::Spacing();

        // Выбор численного метода
        const char *method_items[] = {
            "Euler (order 1)",             // самый простой, разваливается быстро
            "Leapfrog (order 2, symplectic)", // хорош для длинных симуляций
            "RK4 (order 4)",               // стандарт, хорошая точность
            "DP8 (order 8, fixed step)",   // очень точный при фиксированном шаге
            "DP8 Adaptive (order 8)"       // самый точный, шаг подбирается автоматически
        };
        ImGui::Combo("Select method", &method, method_items, IM_ARRAYSIZE(method_items));

        // Подсказка про адаптивный метод
        if (method == 4)
        {
            ImGui::TextDisabled("  Adaptive step: tolerances 1e-10 (rel) / 1e-12 (abs)");
        }

        ImGui::Spacing(); ImGui::Spacing();

        // Кнопка Start (центрированная)
        float buttonWidth = 120.0f;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
        if (ImGui::Button("Start", ImVec2(buttonWidth, 40.0f)))
        {
            // Создаём нужную сцену
            switch (figure)
            {
            case 0: scene = new FigureEightPlotScene(); break;
            case 1: scene = new PolygonPlotScene();     break;
            case 2: scene = new SuperEightPlotScene();  break;
            case 3: scene = new ButterflyPlotScene();   break;
            case 4: scene = new MothPlotScene();        break;
            default: break;
            }

            if (scene)
            {
                // Маппинг индекса метода в enum
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
                std::cout << "Scene created: figure=" << figure
                          << ", N=" << N << ", method=" << method << std::endl;
            }
        }

        ImGui::End();
        ImGui::Render();

        // Очищаем фон и рендерим ImGui-интерфейс
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
        // ---- Режим анимации ----
        // Каждый кадр: шаг физики + отрисовка тел в 3D (всё внутри display())
        scene->display();
    }
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    if (ImGui::GetCurrentContext())
        ImGui::GetIO().DisplaySize = ImVec2((float)w, (float)h);
}

int main(int argc, char **argv)
{
    std::cout << "Program started" << std::endl;

    // Инициализация GLUT: двойная буферизация, RGBA, глубина, мультисэмплинг (сглаживание)
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextVersion(3, 3);   // требуем OpenGL 3.3 core
    glutInitWindowSize(1280, 720);

    GLint window = glutCreateWindow("VichMat lb1");
    if (window == 0)
    {
        std::cerr << "Failed to create window!" << std::endl;
        return 1;
    }

    // GLEW подгружает указатели на функции OpenGL (нужно после создания контекста)
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Инициализация Dear ImGui + ImPlot + ImPlot3D
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImPlot3D::CreateContext();
    ImGui::StyleColorsDark();

    // Бэкенды: GLUT — обработка ввода, OpenGL3 — рендеринг
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui_ImplGLUT_InstallFuncs();  // регистрирует колбеки клавиш, мыши и т.д.

    // glutDisplayFunc — при перерисовке
    // glutIdleFunc — постоянно в простое → непрерывная анимация
    glutDisplayFunc(mainLoop);
    glutReshapeFunc(reshape);
    glutIdleFunc(mainLoop);

    std::cout << "Entering main loop..." << std::endl;
    glutMainLoop();  // не возвращается

    // Недостижимо, но для порядка:
    delete scene;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImPlot::DestroyContext();
    ImPlot3D::DestroyContext();
    ImGui::DestroyContext();

    return 0;
}