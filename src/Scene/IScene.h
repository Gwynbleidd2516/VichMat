#ifndef ISCENE
#define ISCENE

class IScene
{
public:
    virtual void reshape(int w, int h) = 0;
    virtual void keyboard(unsigned char key, int x, int y) = 0;
    virtual void display() = 0;
    virtual void timeFunc(int value) = 0;

    void timer(int value)
    {
        timeFunc(value);
        glutPostRedisplay();
        glutTimerFunc(16, [](int val)
                      {
        IScene* scenePtr = static_cast<IScene*>(glutGetWindowData());
        if (scenePtr) scenePtr->timer(val); }, 0);
    }

};

#endif