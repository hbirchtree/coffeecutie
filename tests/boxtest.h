#ifndef BOXTEST_H
#define BOXTEST_H

#include "opengl/rendering/renderloop.h"
#include "opengl/coffeerenderer.h"
#include "general/common.h"
#include "tests/CubeScape.h"

class BoxTest : public RenderLoop
{
public:
    BoxTest(CoffeeRenderer *renderer);
    ~BoxTest();

private:
    CubeScape* cubescape;
    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;

public:
    std::function<void ()> getInit();
    std::function<void ()> getLoop();
    std::function<void ()> getCleanup();
};

#endif // BOXTEST_H
