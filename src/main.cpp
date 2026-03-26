#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glut.h"
#include "backends/imgui_impl_opengl3.h"

#include "PolygonPlotScene.h"
#include "FigureEightPlotScene.h"

#include "Mmath.h"

static PlotScene *scene = nullptr;

void mainLoop()
{
    if (!scene)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();
        ImGuiIO &io = ImGui::GetIO();

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        // ImGui::SetNextWindowPos(viewport->Pos);

        // Set window flags for a borderless, full-screen appearance
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings;

        {
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowPos(ImVec2(viewport->Size.x * 0.5, viewport->Size.y * 0.5), 0, ImVec2(0.5, 0.5));
            ImGui::Begin("Welcome", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

            {
                const char *text = "Лабораторная работа по вычислительной математике №1";
                ImFont *font = io.Fonts->AddFontFromFileTTF("c:/WINDOWS/Fonts/ARIAL.TTF", 20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
                ImGui::PushFont(font);
                // 1. Вычисляем размер текста
                ImVec2 textSize = ImGui::CalcTextSize(text);

                // 2. Учитываем отступы окна (padding)
                ImVec2 windowSize;
                windowSize.x = textSize.x + ImGui::GetStyle().WindowPadding.x * 2;
                windowSize.y = textSize.y + ImGui::GetStyle().WindowPadding.y * 2; // Пример с заголовком

                ImGui::SetNextWindowPos(ImVec2(viewport->Size.x * 0.5, ImGui::GetWindowPos().y * 1.0), 0, ImVec2(0.5, 0.0));
                ImGui::BeginChild("Title", windowSize, true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);

                ImGui::Text(text);
                ImGui::PopFont();

                ImGui::EndChild();
            }

            {
                const char *text = "Выполнили: Вязников Н.К, Росицкий В.";
                ImFont *font = io.Fonts->AddFontFromFileTTF("c:/WINDOWS/Fonts/ARIAL.TTF", 12.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
                ImGui::PushFont(font);
                // 1. Вычисляем размер текста
                ImVec2 textSize = ImGui::CalcTextSize(text);

                // 2. Учитываем отступы окна (padding)
                ImVec2 windowSize;
                windowSize.x = textSize.x + ImGui::GetStyle().WindowPadding.x * 2;
                windowSize.y = textSize.y + ImGui::GetStyle().WindowPadding.y * 2 + ImGui::GetFrameHeightWithSpacing(); // Пример с заголовком

                // ImGui::SetNextWindowPos(ImVec2(-1, 400.0), 0, ImVec2(0.5, 0.0));
                ImGui::BeginChild("Authors", windowSize, true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);

                ImGui::Text(text);
                ImGui::PopFont();

                ImGui::EndChild();
            }

            static int figure = 0;
            static int N;
            static int method = 0;
            {
                ImFont *font = io.Fonts->AddFontFromFileTTF("c:/WINDOWS/Fonts/ARIAL.TTF", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
                ImGui::PushFont(font);

                const char *items[] = {"Восьмёрка", "N-углоьник"};
                ImGui::Combo("Выберите фигуру", &figure, items, IM_ARRAYSIZE(items));
                // ImGui::SameLine(0.0, 1.0);
                ImGui::InputInt("Количство фигур", &N);
                ImGui::PopFont();
            }

            {
                ImFont *font = io.Fonts->AddFontFromFileTTF("c:/WINDOWS/Fonts/ARIAL.TTF", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
                ImGui::PushFont(font);

                const char *items[] = {"Euler", "Leapfrog", "RK4"};
                ImGui::Combo("Выберите метод", &method, items, IM_ARRAYSIZE(items));
                ImGui::PopFont();
            }

            {
                ImFont *font = io.Fonts->AddFontFromFileTTF("c:/WINDOWS/Fonts/ARIAL.TTF", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
                ImGui::PushFont(font);
                if (ImGui::Button("Запуск"))
                {
                    switch (figure)
                    {
                    case 0:
                        scene = new FigureEightPlotScene;
                        break;
                    case 1:
                        scene = new PolygonPlotScene;
                        break;

                    default:
                        break;
                    }
                    scene->setState(N, (Method)method);
                }
                ImGui::PopFont();
            }

            ImGui::End();
        }
        ImGui::Render();

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        // glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glutSwapBuffers();
        glutPostRedisplay();
    }
    else
        scene->display();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitContextProfile(GLUT_CORE_PROFILE); // optional
    glutInitContextVersion(3, 3);
    glutInitWindowSize(800, 600);

    GLint window = glutCreateWindow("VichMat lb1");
    glLoadIdentity();

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImPlot3D::CreateContext();

    ImGui::StyleColorsDark();


    glutDisplayFunc(mainLoop);

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui_ImplGLUT_InstallFuncs();

    glutMainLoop();

    delete scene;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    return 0;
}