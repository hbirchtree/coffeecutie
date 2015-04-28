#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include "general/common.h"
#include "general/vectors/vector3container.h"
#include "opengl/coffeescene.h"
#include "opengl/components/coffeecamera.h"

class CoffeeRenderer : public QThread
{
    Q_OBJECT
public:
    explicit CoffeeRenderer(QObject *parent = 0, int w = 800, int h = 600);
    CoffeeRenderer(int w = 800, int h = 600);
    ~CoffeeRenderer();

    void setWindowDimensions(int w,int h);
    void setWindowDimensionsValue(int w,int h);
    void setWindowTitle(QString title);
    void setRendererClearColor(glm::vec4 col);
    void requestWindowClose();

    int init();
    int loop();

    //Callback functions
    void printInput(int btn, int action);
    void printInput(double x,double y);

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
    void glfwKeyboardEvent(int key,int action,int modifiers);
    void glfwMouseMoveEvent(double xpos,double ypos);
    void glfwMouseButtonEvent(int btn,int action,int modifiers);
    void glfwMouseScrollEvent(double xoffset,double yoffset);
    void glfwDropEvent(QStringList paths);
    void glfwMouseEnterEvent(bool entered);
    void glfwTypingEvent(QChar character);

public slots:
};

#endif // COFFEERENDERER_H
