#ifndef CAMERA
#define CAMERA

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Camera
{
private:
    vec3 mPosition;
    vec3 mLookAt;
    vec3 mRotate;

public:
    Camera()
    {
        mPosition = vec3(0.0);
        mLookAt = vec3(0.0);
        mRotate = vec3(0.0);
    }

    void setPosition(vec3 pos)
    {
        mPosition = pos;
    }

    void move(vec3 pos)
    {
        mPosition += pos;
    }

    vec3 getPosition()
    {
        return mPosition;
    }

    void setLookAt(vec3 look)
    {
        mLookAt = look;
    }

    void setRotation(vec3 rot)
    {
        mRotate = rot;
    }

    void rotate(vec3 rot)
    {
        mRotate += rot;
    }

    vec3 getRotation() const
    {
        return mRotate;
    }

    mat4 getViewMatrix()
    {
        return glm::lookAt(mPosition, mLookAt, vec3(0.0, 1.0, 0.0));
    }

    mat4 getProjectionMatrix()
    {
        return ortho(-1.0f, 1.0f, -1.0f, 1.0f, -2.0f, 2.0f);
    }

    mat4 getTarnsformedModel(mat4 model)
    {
        model = glm::rotate(model, glm::radians(mRotate.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Example rotation
        model = glm::rotate(model, glm::radians(mRotate.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Example rotation
        model = glm::rotate(model, glm::radians(mRotate.z), glm::vec3(1.0f, 0.0f, 1.0f)); // Example rotation
        model = glm::translate(model, mPosition);                                           // Example translation
        return model;
    }
};

#endif