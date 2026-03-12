#ifndef ISCENE
#define ISCENE

class IScene
{
public:
    virtual void reshape(int w, int h) = 0;
    virtual void keyboard(unsigned char key, int x, int y) = 0;
    virtual void display() = 0;
    virtual void timer(int value) = 0;
};

#endif