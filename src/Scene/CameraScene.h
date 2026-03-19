#ifndef CAMERA_SCENE
#define CAMERA_SCENE

#include "IScene.h"
#include "Camera.h"

class CameraScene : public IScene
{
protected:
    Camera mCamera;
    vec2 mPrevMousePos;
    float mSense = 0.005;

public:
    CameraScene()
    {
        mPrevMousePos = vec2(0);
    }

    void keyboard(unsigned char key, int x, int y) override
    {
        MousePassiveMotion(mPrevMousePos.x - x, mPrevMousePos.y - y);
        mPrevMousePos.x = x;
        mPrevMousePos.y = y;

        if (key == 27) // Escape key
            glutLeaveMainLoop();
    }

    void reshape(int w, int h) override
    {
        glViewport(0, 0, w, h);
    }

private:
    void MousePassiveMotion(int x, int y)
    {
        float dx = (float)x * mSense;
        float dy = (float)y * -mSense;
        float radius = 1.0f;
        float eyeX = radius * cos(dy) * sin(dx);
        float eyeY = radius * sin(dy); // Or cos(theta) * sin(phi) depending on coordinate system
        float eyeZ = radius * cos(dy) * cos(dx);

        mCamera.setPosition(vec3(eyeX, eyeY, eyeZ));
    }
};

#endif