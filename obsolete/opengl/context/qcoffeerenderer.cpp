#include "qcoffeerenderer.h"

#ifdef QOPENGL_CONTEXT_MANAGER

#include <QOpenGLWindow>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>

QCoffeeRenderer::QCoffeeRenderer(QObject *parent) : CoffeeRenderer(parent)
{

}

QCoffeeRenderer::QCoffeeRenderer(QObject *parent, int w, int h) : CoffeeRenderer(parent)
{
    setWindowDimensions(QSize(w,h));
}

QCoffeeRenderer::QCoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state) : CoffeeRenderer(parent,w,h)
{
    startmode = state;
}

QCoffeeRenderer::QCoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state, QString windowTitle) : CoffeeRenderer(parent,w,h,state)
{
    this->windowTitle = windowTitle;
}

int QCoffeeRenderer::init()
{
    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setSwapInterval(1);
    window = new QCoffeeWindow();
    window->show();
    return 0;
}

int QCoffeeRenderer::loop()
{
    while(window)
        window->render();
    return 0;
}


QCoffeeWindow::QCoffeeWindow() : QWindow()
{
}

QCoffeeWindow::~QCoffeeWindow()
{
    cleanup();
}

void QCoffeeWindow::initialize()
{
    context->setFormat(format);
    context->create();
    context->makeCurrent(this);

    if(procedure)
        (*procedure->getInit())();
}

void QCoffeeWindow::render()
{
    if(procedure)
        (*procedure->getLoop())();
}

void QCoffeeWindow::cleanup()
{
    if(procedure)
        (*procedure->getCleanup())();
}

QPointer<RenderLoop> QCoffeeWindow::getProcedure() const
{
    return procedure;
}

void QCoffeeWindow::setProcedure(const QPointer<RenderLoop> &value)
{
    procedure = value;
}

#endif
