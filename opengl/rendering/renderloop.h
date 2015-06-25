#ifndef RENDERLOOP
#define RENDERLOOP

#include <QObject>
class RenderLoop{
public:
    RenderLoop(){}
    ~RenderLoop(){}

    virtual std::function<void()> *getInit() = 0;
    virtual std::function<void()> *getLoop() = 0;
    virtual std::function<void()> *getCleanup() = 0;
};

#endif // RENDERLOOP
