#ifndef CUBE_BOUNDING_SCENE
#define CUBE_BOUNDING_SCENE

#include "IScene.h"
#include "Cube.h"
#include "Camera.h"
#include <glm/ext/scalar_constants.hpp>

class CubeScene : public IScene
{
private:
    Cube mObj;
    Camera mCamera;

public:
    CubeScene()
    {
        mCamera.setPosition(vec3(0.0, 0.0, 1.0));
        // mCamera.rotate(vec3(45.0, 45.0, 45.0));
        // mCamera.setPosition(vec3(-sinf(mCamera.getRotation().x * (pi<float>() / 180)) * cosf((mCamera.getRotation().y) * (pi<float>() / 180)),
        //                          -sinf((mCamera.getRotation().y) * (pi<float>() / 180)),
        //                          cosf((mCamera.getRotation().x) * (pi<float>() / 180)) * cosf((mCamera.getRotation().y) * (pi<float>() / 180))));
    }

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
        glm::mat4 model = glm::mat4(1.0f);
        // Apply transformations (scaling, rotation, translation)
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));                   // Example translation
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Example rotation
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                       // Example scale

        // 2. Create the View matrix (V)
        // glm::lookAt is commonly used to create a view matrix
        glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Point the camera at the origin
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);     // Up direction
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);

        // 3. Create the Projection matrix (P)
        // glm::perspective for a perspective projection
        float fov = glm::radians(45.0f); // 45 degree field of view
        float aspect = (float)800 / (float)600;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        glm::mat4 projection = glm::perspective(fov, aspect, nearPlane, farPlane);
        // Or glm::ortho for an orthographic projection
        // glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, 0.1f, 100.0f);

        // 4. Combine them to form the MVP matrix
        glm::mat4 mvp = projection * view * model;
        // mObj.setView(mvp);
        mObj.render(mvp);
        glutSwapBuffers(); // Swap the front and back buffers
    }

    void timeFunc(int value) override
    {
    }
};

#endif