#ifndef TEST_SCENE
#define TEST_SCENE

#include "IScene.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "TestObject.h"

#include<glm/glm.hpp>

class TestScene : public IScene
{
private:
    TestObject test;

public:
    // TestScene(/* args */);

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
        test.render(glm::mat4(1.0f));
        glutSwapBuffers(); // Swap the front and back buffers
    }

    void timeFunc(int value) override
    {

    }

    // ~TestScene();
};

#endif