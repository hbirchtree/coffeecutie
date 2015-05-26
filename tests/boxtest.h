#ifndef BOXTEST_H
#define BOXTEST_H

#include "opengl/rendering/renderloop.h"
#include "opengl/context/coffeerenderer.h"
#include "general/common.h"
#include "tests/CubeScape.h"

class BoxTest : public RenderLoop
{
public:
    BoxTest(CoffeeRenderer *renderer);
    ~BoxTest();

private:
    double timer = 0.0;
    CubeScape* cubescape;
    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;

public:
    std::function<void ()> *getInit();
    std::function<void ()> *getLoop();
    std::function<void ()> *getCleanup();

    // RenderLoop interface
public:
    QList<QObject *> getThreadObjects() const;
};

#endif // BOXTEST_H
