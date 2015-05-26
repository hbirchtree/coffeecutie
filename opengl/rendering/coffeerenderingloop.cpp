#include "coffeerenderingloop.h"

CoffeeRenderingLoop::CoffeeRenderingLoop(std::function<void()> init, std::function<void()> loop, std::function<void()> cleanup) : RenderLoop(0)
{
    this->_rendering_loop_init = init;
    this->_rendering_loop = loop;
    this->_rendering_loop_cleanup = cleanup;
}

CoffeeRenderingLoop::~CoffeeRenderingLoop()
{
}

std::function<void ()> *CoffeeRenderingLoop::getInit()
{
    return &_rendering_loop_init;
}

std::function<void ()> *CoffeeRenderingLoop::getLoop()
{
    return &_rendering_loop;
}

std::function<void ()> *CoffeeRenderingLoop::getCleanup()
{
    return &_rendering_loop_cleanup;
}

QList<QObject *> CoffeeRenderingLoop::getThreadObjects() const
{
    return QList<QObject*>();
}
