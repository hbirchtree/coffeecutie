#include "coffeerendererbase.h"

#include <QDebug>

CoffeeRendererBase::CoffeeRendererBase(QObject *parent) : QObject(parent)
{
}

void CoffeeRendererBase::run()
{
    int stat = init();
    rendererInitFinished();
    if(stat!=0)
        qFatal("Failed to initialize renderer!");
    stat = loop();
    if(stat!=0)
        qFatal("Loop function failed!");

    qDebug("Stopping rendering thread");
    rendererShuttingDown();
}

void CoffeeRendererBase::setWindowState(Qt::WindowState windowState)
{
    if (m_windowState == windowState)
        return;

    m_windowState = windowState;
    emit windowStateChanged(windowState);
}

void CoffeeRendererBase::setWindowTitle(const QString &windowTitle)
{
    m_windowTitle = windowTitle;
}

void CoffeeRendererBase::setStartDisplay(uint startDisplay)
{
    m_startDisplay = startDisplay;
}

void CoffeeRendererBase::setLoop(RenderLoop *obj)
{
    m_loop = obj;
}

void CoffeeRendererBase::setWindowDimensions(const QSize &windowSize)
{
    m_windowSize = windowSize;
}

void CoffeeRendererBase::setFramebufferSize(const QSize &size)
{
    this->m_framebufferSize = size;
}

void CoffeeRendererBase::openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    Q_UNUSED(id)
    Q_UNUSED(userParam)
    if(severity==GL_DEBUG_SEVERITY_NOTIFICATION)
        return;
    QByteArray messageB;
    messageB.setRawData(message,length);
    qWarning("OpenGL debug callback:\n%s(%s):%s:\n%s",
           glbinding::Meta::getString(source).c_str(),
           glbinding::Meta::getString(type).c_str(),
           glbinding::Meta::getString(severity).c_str(),
           messageB.toStdString().c_str());
}

Qt::WindowState CoffeeRendererBase::windowState() const
{
    return m_windowState;
}

QSize CoffeeRendererBase::getCurrentFramebufferSize() const
{
    return m_framebufferSize;
}

QSize CoffeeRendererBase::getWindowDimensions() const
{
    return m_windowSize;
}

QString CoffeeRendererBase::getWindowTitle() const
{
    return m_windowTitle;
}

uint CoffeeRendererBase::startDisplay() const
{
    return m_startDisplay;
}
