#include "boxtest.h"

BoxTest::BoxTest(CoffeeRenderer* renderer){
    _rendering_loop_init = [this,renderer](){
        renderer->setSamples(4);
        renderer->setSwapInterval(0);
        renderer->updateRendererClearColor(glm::vec4(0,0.2,0.2,1));
        cubescape = new CubeScape();
        QSize s = renderer->getCurrentFramebufferSize();
        cubescape->resize(s.width(), s.height());
    };
    renderer->connect(renderer,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent ev){
        cubescape->resize(ev.size().width(),ev.size().height());
    });
    _rendering_loop = [this,renderer](){
        cubescape->draw();
    };
    _rendering_loop_cleanup = [this,renderer](){
        delete cubescape;
        cubescape = nullptr;
    };
    renderer->connect(renderer,&CoffeeRenderer::winClose,[=](){
        qDebug("Window closing request received");
        renderer->requestWindowClose();
    });
    renderer->connect(renderer,&CoffeeRenderer::winKeyboardEvent,[=](QKeyEvent event){
        if(event.key()==GLFW_KEY_ESCAPE&&event.type()==QEvent::KeyPress)
            renderer->requestWindowClose();
    });
    renderer->connect(renderer,&CoffeeRenderer::contextReportFrametime,[=](float frametime){
        if(glfwGetTime()>=timer){
            qDebug("FPS: %.0f",1.f/frametime);
            timer = glfwGetTime()+1.0;
        }
    });
}

BoxTest::~BoxTest()
{
}

std::function<void ()> *BoxTest::getInit()
{
    return &_rendering_loop_init;
}

std::function<void ()> *BoxTest::getLoop()
{
    return &_rendering_loop;
}

std::function<void ()> *BoxTest::getCleanup()
{
    return &_rendering_loop_cleanup;
}

QList<QObject *> BoxTest::getThreadObjects() const
{
    return QList<QObject*>();
}
