#ifndef CIRCLE_BOUNDING_SCENE
#define CIRCLE_BOUNDING_SCENE

#include "CameraScene.h"
#include "Circle.h"
#include "Point.h"
#include <glm/ext/scalar_constants.hpp>

class CircleBoundingScene : public CameraScene
{
private:
    Circle mObj;
    Circle mObj2;
    float mTime = 0.0;

public:
    CircleBoundingScene()
    {
        mCamera.setRotation(vec3(0.0, 0.0, 0.0));
        mCamera.setPosition(vec3(0.0f, 0.0f, 1.0f));
        mCamera.setLookAt(vec3(0.0f, 0.0f, 0.0f));
        mObj.setPosition(vec2(0.0, 0.0));
        mObj2.setPosition(vec2(0.4, 0.4));
    }

    void display() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp = mCamera.getProjectionMatrix() * mCamera.getViewMatrix() * mCamera.getTarnsformedModel(glm::mat4(1.0f));

        mObj.render(mvp);
        mObj2.render(mvp);
        glutSwapBuffers(); // Swap the front and back buffers
    }

    void timeFunc(int value) override
    {
        // mTime += 0.1;
    }
};

#endif