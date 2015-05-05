#include "qcoffeewindow.h"

#include <QOpenGLContext>
#include <QOpenGLPaintDevice>

QCoffeeWindow::QCoffeeWindow(QWindow* parent,QSurfaceFormat format) : QWindow(parent)
{
    setSurfaceType(QWindow::OpenGLSurface);
    context = new QOpenGLContext(this);
    create();
    initialize();
    this->format = format;
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
        procedure->getInit()();
}

void QCoffeeWindow::render()
{
    if(procedure)
        procedure->getLoop()();
}

void QCoffeeWindow::cleanup()
{
    if(procedure)
        procedure->getCleanup()();
}

QPointer<RenderLoop> QCoffeeWindow::getProcedure() const
{
    return procedure;
}

void QCoffeeWindow::setProcedure(const QPointer<RenderLoop> &value)
{
    procedure = value;
}

