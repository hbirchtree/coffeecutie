#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include <QCoreApplication>
#include "general/common.h"
#include "opengl/rendering/renderloop.h"
#include <QMimeData>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMoveEvent>

class GLFWwindow;
class RenderLoop;

class CoffeeRenderer : public QObject, public QRunnable
{
    Q_PROPERTY(bool mouseGrabbing READ isMouseGrabbed)
    Q_PROPERTY(int openGLSamples READ getSamples)
    Q_PROPERTY(double loopTime READ getLoopTime)
    Q_PROPERTY(QSize windowSize READ getWindowDimensions)
    Q_PROPERTY(QSize framebufferSize READ getCurrentFramebufferSize)
    Q_PROPERTY(QString windowTitle READ getWindowTitle)
    Q_PROPERTY(double frameTime READ getLatestFrameTime)

    Q_OBJECT
public:
    enum RendererExitStatus {
        Undefined,
        NoLoopObject,
        FailedToCreateWindow,
        FailedToInitGLFW
    };

    CoffeeRenderer(QObject *parent);
    CoffeeRenderer(QObject *parent, int w, int h);
    CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state);
    CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state, QString windowTitle);
    ~CoffeeRenderer();

    virtual int init();
    virtual int loop();


    virtual int getStartDisplay() const;
    virtual double getLoopTime() const;
    virtual QString getWindowTitle() const;
    virtual QSize getWindowDimensions() const;
    virtual QSize getCurrentFramebufferSize() const;
    virtual QSize *getFramebufferSizePt();
    virtual int getMouseInputMode() const;
    virtual bool isMouseGrabbed() const;
    virtual double getLatestFrameTime() const;

    virtual int getSamples() const;

public slots:
    virtual void setSwapInterval(uint interval);
    virtual void setStartDisplay(uint value);
    virtual void setWindowDimensions(const QSize &value);
    virtual void setSamples(uint value);
    virtual void setLoop(RenderLoop *value);
    virtual void requestWindowClose();
    virtual void updateWindowTitle(QString value);
    virtual void updateRendererClearColor(glm::vec4 value);
    virtual void updateWindowDimensions(QSize dims);
    virtual void setWindowState(Qt::WindowState state);
    virtual void setStartmode(const Qt::WindowState &value);
    virtual void updateMouseGrabbing(bool state);
    virtual void setMousePos(int x,int y);

    virtual void run();

private slots:
    GLFWwindow *setWindowedFullscreen(uint monitor);
    GLFWwindow *setFullscreen(uint monitor);
    GLFWwindow *setWindowed();

protected:
    QPointer<RenderLoop> loopObject;
    //Settings
    int samples = 0;
    QSize windowDimensions;
    QSize framebufferSize;
    QString windowTitle = "Qt Café";
    Qt::WindowState startmode = Qt::WindowNoState;
    int startDisplay = 0;

private:
    //GLFW objects
    GLFWwindow* window;

    double frametime;

signals:
    void rendererFailed(RendererExitStatus status);

    //Renderer events
    void windowTitleUpdated(QString title);
    void clearColorChanged(glm::vec4 color);

    //Input events
    void winMouseGrabbed(QEvent event);
    void winKeyboardEvent(QKeyEvent event);
    void winMouseEvent(QMouseEvent event);
    void winWheelEvent(QWheelEvent event);
    void winDropEvent(QPointer<QMimeData> data); //Temporary workaround until QDropEvent decides to work. Delete the QMimeData object!
    void winMouseEnterEvent(QEvent event);

    void winResize(QResizeEvent event);
    void winPosChanged(QMoveEvent event);
    void winFocusChanged(QFocusEvent event);
    void winRefresh();
    void winFrameBufferResize(QResizeEvent event);
    void winClose();
    void winStateChanged(QWindowStateChangeEvent event);

    void contextReportFrametime(float framerate);
};

#endif // COFFEERENDERER_H
