#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include "general/common.h"
#include "opengl/rendering/renderloop.h"
#include <QCoreApplication>
#include <QMimeData>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMoveEvent>

class CoffeeRenderer : public QObject, public QRunnable
{
    Q_PROPERTY(bool mouseGrabbing READ isMouseGrabbed)
    Q_PROPERTY(int openGLSamples READ getSamples)
    Q_PROPERTY(double loopTime READ getLoopTime)
    Q_PROPERTY(QSize windowSize READ getWindowDimensions)
    Q_PROPERTY(QSize framebufferSize READ getCurrentFramebufferSize)
    Q_PROPERTY(QString windowTitle READ getWindowTitle)

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

    int init();
    int loop();


    int getStartDisplay() const;
    double getLoopTime() const;
    QString getWindowTitle() const;
    QSize getWindowDimensions() const;
    QSize getCurrentFramebufferSize() const;
    QSize *getFramebufferSizePt();
    int getMouseInputMode() const;
    bool isMouseGrabbed() const;

    int getSamples() const;

public slots:
    void setSwapInterval(uint interval);
    void setStartDisplay(uint value);
    void setWindowDimensions(const QSize &value);
    void setSamples(uint value);
    void setLoop(RenderLoop *value);
    void requestWindowClose();
    void updateWindowTitle(QString value);
    void updateRendererClearColor(glm::vec4 value);
    void updateWindowDimensions(QSize dims);
    void setWindowState(Qt::WindowState state);
    void setStartmode(const Qt::WindowState &value);
    void updateMouseGrabbing(bool state);
    void setMousePos(int x,int y);

    void run();

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
