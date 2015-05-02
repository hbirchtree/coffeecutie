#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include "general/common.h"
#include "opengl/rendering/renderloop.h"
#include <QMimeData>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMoveEvent>

class CoffeeRenderer : public QThread
{
    Q_OBJECT
public:
    CoffeeRenderer(QObject *parent);
    CoffeeRenderer(QObject *parent,int w, int h);
    CoffeeRenderer(QObject *parent,int w, int h, Qt::WindowState state);
    CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state,QString windowTitle);
    ~CoffeeRenderer();

    int init();
    int loop();

    int getStartDisplay() const;
    void setStartDisplay(int value);
    QSize getWindowDimensions() const;
    void setWindowDimensions(const QSize &value);

    QSize getCurrentFramebufferSize();

    QSize *getFramebufferSizePt();

    void setLoop(RenderLoop *value);

    int getSamples() const;
    void setSamples(int value);

public slots:
    void requestWindowClose();
    void updateWindowTitle(QString value);
    void updateRendererClearColor(glm::vec4 value);
    void updateWindowDimensions(int w,int h);
    void setWindowState(Qt::WindowState state);
    void setStartmode(const Qt::WindowState &value);
    void updateMouseGrabbing(bool state);

private slots:
    GLFWwindow *setWindowedFullscreen(int monitor);
    GLFWwindow *setFullscreen(int monitor);
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

    void contextReportFramerate(float framerate);

};

#endif // COFFEERENDERER_H
