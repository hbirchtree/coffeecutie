#include "boxtest.h"

BoxTest::BoxTest(CoffeeRenderer* renderer){
    _rendering_loop_init = [this,renderer](){
        renderer->setSamples(4);
        renderer->updateRendererClearColor(glm::vec4(0,0.2,0.2,1));
        cubescape = new CubeScape();
        QSize s = renderer->getCurrentFramebufferSize();
        cubescape->resize(s.width(), s.height());
    };
    connect(renderer,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent ev){
        cubescape->resize(ev.size().width(),ev.size().height());
    });
    _rendering_loop = [this,renderer](){
        cubescape->draw();
    };
    _rendering_loop_cleanup = [this,renderer](){
        delete cubescape;
        cubescape = nullptr;
    };
}

BoxTest::~BoxTest()
{
}

std::function<void ()> BoxTest::getInit()
{
    return _rendering_loop_init;
}

std::function<void ()> BoxTest::getLoop()
{
    return _rendering_loop;
}

std::function<void ()> BoxTest::getCleanup()
{
    return _rendering_loop_cleanup;
}

