#include "coffeerenderer.h"

#include <QCoreApplication>
#include "opengl/rendering/renderloop.h"
#include <QMimeData>

#include "engine/scripting/coffeeinputevent.h"
#include "opengl/rendering/renderloop.h"

CoffeeRenderer::CoffeeRenderer(QObject *parent) : CoffeeGLFWContextManager(parent)
{
    connect(this,&CoffeeRenderer::winMouseEnterEvent,[=](QEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });
    connect(this,&CoffeeRenderer::winMouseEvent,[=](QMouseEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });
    connect(this,&CoffeeRenderer::winMouseGrabbed,[=](QEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });
    connect(this,&CoffeeRenderer::winWheelEvent,[=](QWheelEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });

    connect(this,&CoffeeRenderer::winKeyboardEvent,[=](QKeyEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });
}

CoffeeRenderer::CoffeeRenderer(QObject *parent, int w, int h) : CoffeeRenderer(parent)
{
    setWindowDimensions(QSize(w,h));
}

CoffeeRenderer::CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state) : CoffeeRenderer(parent,w,h)
{
    setWindowState(state);
}

CoffeeRenderer::CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state, QString windowTitle) : CoffeeRenderer(parent,w,h,state)
{
    setWindowTitle(windowTitle);
}

CoffeeRenderer::~CoffeeRenderer()
{
}

/*
 * 01 : no rendering process specified
 */
int CoffeeRenderer::loop(){
    if(!m_loop){
        qFatal("No loop object defined!");
    }
    qDebug("Initializing loop");

    std::function<void()> *_init = m_loop->init();
    std::function<void()> *_loop = m_loop->loop();
    std::function<void()> *_cleanup = m_loop->cleanup();

    double frametime = 0.0;

    qDebug("Running initialization function");
    (*_init)();

    {
        emit winFrameBufferResize(QResizeEvent(getWindowDimensions(),getWindowDimensions()));
    }

    qDebug("Running loop function");
    while(!shouldWindowClose()){

        checkGpuMemoryStatistics();

        frametime = getLoopTime();
        pollEvents();
        (*_loop)();
        executeRunQueue();
        swapBuffers();
        setLatestFrameTime(getLoopTime()-frametime);
        contextReportFrametime(getLatestFrameTime());
    }
    qDebug("Running cleanup function");
    (*_cleanup)();

    qDebug("Estimated uptime: %.1f seconds",getLoopTime());
    return 0;
}

void CoffeeRenderer::getVideoMemoryUsage(qint32 *current, qint32 *total) const
{
    CoffeeGPUMonitor::getGpuMemoryUsage(current,total);
}

void CoffeeRenderer::requestMemoryCheck()
{
    CoffeeGPUMonitor::requestGpuMemoryCheck();
}

void CoffeeRenderer::flushPipeline(){
    glFlush();
}

void CoffeeRenderer::setClearColor(float r, float g, float b, float a)
{
    queueFunction(new std::function<void()>([=](){
        glClearColor(r,g,b,a);
    }));
}
