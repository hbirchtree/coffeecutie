#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include "general/common.h"
#include "general/vectors/vector3container.h"
#include "opengl/coffeescene.h"
#include "opengl/components/coffeecamera.h"
#include "general/coffeejoystick.h"
#include <QMimeData>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

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

    QSize *getFramebufferSizePt();

public slots:
    void requestWindowClose();
    void updateWindowTitle(QString value);
    void updateRendererClearColor(glm::vec4 value);
    void updateWindowDimensions(int w,int h);
    void setWindowState(Qt::WindowState state);
    void setStartmode(const Qt::WindowState &value);

private slots:
    GLFWwindow *setWindowedFullscreen(int monitor);
    GLFWwindow *setFullscreen(int monitor);
    GLFWwindow *setWindowed();

private:
    //Settings
    int samples = 0;
    QSize windowDimensions;
    QSize framebufferSize;
    QString windowTitle = "Qt Café";
    Qt::WindowState startmode = Qt::WindowNoState;
    int startDisplay = 0;

    //OpenGL
    glm::vec4 clearColor;

    //GLFW objects
    GLFWwindow* window;

    //Data source
    QPointer<CoffeeScene> scene;

signals:
    //Renderer events
    void windowDimensionsValueUpdated();
    void windowTitleValueUpdated();
    void clearColorValueChanged();

    //Input events
    void glfwKeyboardEvent(QKeyEvent event);
    void glfwMouseEvent(QMouseEvent event);
    void glfwWheelEvent(QWheelEvent event);
    void glfwDropEvent(QPointer<QMimeData> data); //Temporary workaround until QDropEvent decides to work. Delete the QMimeData object!
    void glfwMouseEnterEvent(QEvent event);

    void glfwWinResize(QResizeEvent event);
    void glfwWinPosChanged(QMoveEvent event);
    void glfwWinFocusChanged(QFocusEvent event);
    void glfwWinRefresh();
    void glfwFrameBufferResize(QResizeEvent event);
    void glfwWinClose();
    void glfwWinStateChanged(QWindowStateChangeEvent event);

public slots:
};

#endif // COFFEERENDERER_H
