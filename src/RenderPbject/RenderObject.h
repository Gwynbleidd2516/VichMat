#ifndef RENDER_OBJECT
#define RENDER_OBJECT

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shader.h"

class RenderObject
{
protected:
    Shader mShader;
    GLuint mVBO; // Vertex Buffer Object
    GLuint mVAO; // Vertex Array Object
    GLuint mEBO;

    unsigned int mCount;

public:
    virtual void render() const;
};

#endif