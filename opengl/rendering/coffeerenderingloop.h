#ifndef COFFEERENDERINGLOOP_H
#define COFFEERENDERINGLOOP_H

#include "opengl/rendering/renderloop.h"
#include "general/common.h"
class CoffeeRenderingLoop : public RenderLoop
{
public:
    CoffeeRenderingLoop(std::function<void()> _rendering_loop_init, std::function<void()> _rendering_loop,std::function<void()> _rendering_loop_cleanup);
    ~CoffeeRenderingLoop();

    // RenderLoop interface
    std::function<void ()> getInit();
    std::function<void ()> getLoop();
    std::function<void ()> getCleanup();

private:
    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;
};

#endif // COFFEERENDERINGLOOP_H
