#ifndef COFFEERENDERERBASE_H
#define COFFEERENDERERBASE_H

#include <QObject>
#include <QRunnable>

class QMimeData;
class QWindowStateChangeEvent;
class RenderLoop;
class CoffeeInputEvent;

#include "general/common.h"

#include <QSize>
#include <QPointF>
#include <QEvent>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QDropEvent>

class CoffeeRendererBase : public QObject, public QRunnable
{
    Q_PROPERTY(bool mouseGrabbing READ isMouseGrabbed WRITE setMouseGrabbing)

    Q_PROPERTY(QSize windowSize READ getWindowDimensions WRITE setWindowDimensions)
    Q_PROPERTY(QSize framebufferSize READ getCurrentFramebufferSize)
    Q_PROPERTY(QString windowTitle READ getWindowTitle WRITE setWindowTitle)

    Q_PROPERTY(Qt::WindowState windowState READ windowState WRITE setWindowState NOTIFY windowStateChanged)
    Q_PROPERTY(uint startDisplay READ startDisplay WRITE setStartDisplay)

    Q_OBJECT
    Qt::WindowState m_windowState = Qt::WindowNoState;
    QSize m_framebufferSize;
    QSize m_windowSize;
    QString m_windowTitle;
    uint m_startDisplay;

public:
    enum RendererMessageSeverity
    {
        NotSupposedToHappen,

        InformationMessage,
        WarningMessage,
        FatalMessage,
    };
    Q_ENUMS(RendererMessageSeverity)

    enum RendererExitStatus {
        Undefined,
        NoLoopObject,
        FailedToCreateWindow,
        FailedToInitSystem
    };
    Q_ENUMS(RendererExitStatus)

    virtual int init() = 0;
    virtual int loop() = 0;

    static void APIENTRY openGLDebugCallback(GLenum source, GLenum type,
                                             GLuint id, GLenum severity,
                                             GLsizei length,
                                             const GLchar* message,
                                             const void* userParam);

    virtual Qt::WindowState windowState() const;
    virtual QSize getCurrentFramebufferSize() const;
    virtual QSize getWindowDimensions() const;
    virtual QString getWindowTitle() const;
    uint startDisplay() const;

    virtual bool isMouseGrabbed() const = 0;

    virtual QWindow* windowHandle() = 0;

public slots:
    virtual void run();

    void setStartDisplay(uint startDisplay);
    virtual void setWindowState(Qt::WindowState windowState);

    virtual void setWindowTitle(const QString &windowTitle);
    virtual void requestWindowClose() = 0;
    virtual void setWindowDimensions(const QSize &windowSize);
    virtual void setSwapInterval(uint interval) = 0;

    virtual void setLoop(RenderLoop* obj);

    virtual void setMouseGrabbing(bool mode) = 0;
    virtual void setMousePosition(QPointF pos) = 0;

    virtual void setClearColor(float r, float g, float b, float a) = 0;

    virtual void showWindow() = 0;
    virtual void hideWindow() = 0;

protected slots:
    virtual void setFramebufferSize(const QSize& size);

    virtual void swapBuffers() = 0;
    virtual void pollEvents() = 0;

signals:
    //Input events
    void inputEvent(CoffeeInputEvent* ev);

    void winMouseGrabbed(QEvent event);
    void winKeyboardEvent(QKeyEvent event);
    void winMouseEvent(QMouseEvent event);
    void winWheelEvent(QWheelEvent event);
    void winDropEvent(QDropEvent event);
    void winMouseEnterEvent(QEvent event);

    void winResize(QResizeEvent event);
    void winPosChanged(QMoveEvent event);
    void winFocusChanged(QFocusEvent event);
    void winRefresh();
    void winFrameBufferResize(QResizeEvent event);
    void windowAspectChanged(float aspect);
    void winClose();

    void windowStateChanged(QWindowStateChangeEvent event);
    void windowStateChanged(Qt::WindowState event);

    void contextReportFrametime(float framerate);

    void rendererInitFinished();
    void rendererShuttingDown();

    void renderingErrorOccurred(QString source,   QString type,
                                QString severity, QString message);

    void rendererMessage(uint severity, QString message); //TODO : Switch it to RendererMessageSeverity type at some point.

protected:
    CoffeeRendererBase(QObject *parent);

    RenderLoop *m_loop;
};

#endif // COFFEERENDERERBASE_H
