#ifndef EMPTY_SCENE
#define EMPTY_SCENE

#include "IScene.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

class EmptyScene : public IScene
{
public:
    void reshape(int w, int h) override
    {
        glViewport(0, 0, w, h);
    }
    void keyboard(unsigned char key, int x, int y) override
    {
        if (key == 27) // Escape key
            glutLeaveMainLoop();
    }
    void display() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glutSwapBuffers(); // Swap the front and back buffers
    }

    void timer(int value) override
    {

    }
};

#endif