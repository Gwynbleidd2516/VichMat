#ifndef ISCENE
#define ISCENE

#include <GL/glew.h>
#include <GL/freeglut.h>

// Интерфейс сцены — чистый абстрактный класс.
// Все типы сцен обязаны реализовать display(), который вызывается каждый кадр.
class IScene
{
public:
    virtual void display() = 0;
};

#endif
