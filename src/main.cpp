#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>

#include "EmptyScene.h"
#include "TestScene.h"
#include "CircleBoundingScene.h"
#include "CubeScene.h"

void setScene(IScene &scene)
{
    glutSetWindowData(&scene);

    glutDisplayFunc([]()
                    { IScene* scenePtr = static_cast<IScene*>(glutGetWindowData());
                        if (scenePtr) scenePtr->display(); });
    glutReshapeFunc([](int w, int h)
                    {
        IScene* scenePtr = static_cast<IScene*>(glutGetWindowData());
        if (scenePtr) scenePtr->reshape(w, h); });

    glutKeyboardFunc([](unsigned char key, int x, int y)
                     {
        IScene* scenePtr = static_cast<IScene*>(glutGetWindowData());
        if (scenePtr) scenePtr->keyboard(key, x, y); });

    glutTimerFunc(16, [](int val)
                  {
        IScene* scenePtr = static_cast<IScene*>(glutGetWindowData());
        if (scenePtr) scenePtr->timer(val); }, 0);
}

int main(int argc, char **argv)
{
    std::cout << argv[0] << std::endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitContextProfile(GLUT_CORE_PROFILE); // optional
    glutInitWindowSize(800, 600);
    glLoadIdentity();

    GLint window = glutCreateWindow("FreeGLUT Shader Example");
    glutInitContextProfile(window);

    // Initialize GLEW (must be done after window creation)
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    if (!GLEW_VERSION_2_0)
    {
        fprintf(stderr, "OpenGL 2.0 or higher is required.\n");
        return -1;
    }

    CircleBoundingScene scene;

    setScene(scene);

    glutMainLoop();

    return 0;
}
