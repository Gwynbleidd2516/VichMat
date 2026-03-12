#ifndef CIRCLE_BOUNDING_SCENE
#define CIRCLE_BOUNDING_SCENE

#include "IScene.h"
#include "Circle.h"

class CircleBoundingScene : public IScene
{
private:
    Circle mObj;
    float mTime = 0.0;

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
        mObj.setPosition(glm::vec2(0.0, std::sinf(mTime)));
        mObj.render();
        glutSwapBuffers(); // Swap the front and back buffers
    }

    void timer(int value) override
    {
        mTime += 0.1;
        glutPostRedisplay();
        glutTimerFunc(16, [](int val)
                      {
        IScene* scenePtr = static_cast<IScene*>(glutGetWindowData());
        if (scenePtr) scenePtr->timer(val); }, 0);
    }
};

#endif