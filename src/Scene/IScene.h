#ifndef ISCENE
#define ISCENE

#include <GL/glew.h>
#include <GL/freeglut.h>

class IScene
{
public:
    virtual void display() = 0;
};

#endif