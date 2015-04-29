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
    CoffeeRenderer(QObject *parent, int w, int h);
    ~CoffeeRenderer();

    void setWindowDimensions(int w,int h);
    void setWindowDimensionsValue(int w,int h);
    void setWindowTitle(QString title);
    void setRendererClearColor(glm::vec4 col);
    void requestWindowClose();

    int init();
    int loop();

    QPointer<CoffeeCamera> camera;

private:
    //Settings
    int samples = 0;
    int width = 1280;
    int height = 720;
    QString windowTitle = "Qt Café";

    //OpenGL
    glm::vec4 clearColor;

    //GLFW objects
    GLFWwindow* window;

    //Data source
    QPointer<CoffeeScene> scene;

    void updateWindowTitle(QString value);
    void updateRendererClearColor(glm::vec4 value);
    void updateWindowDimensions(int w,int h);

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

public slots:
};

#endif // COFFEERENDERER_H
