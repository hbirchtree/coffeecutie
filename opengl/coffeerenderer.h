#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include "general/common.h"
#include "general/vectors/vector3container.h"
#include "opengl/coffeescene.h"

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
    void printInput(int btn, int action){
        if(btn==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)
            requestWindowClose();
        if(btn==GLFW_MOUSE_BUTTON_1&&action==GLFW_PRESS)
            setRendererClearColor(glm::vec4(1,1,1,0));
        qDebug() << "INPUT: "+QString::number(btn)+" : "+QString::number(action);
    }
    void printInput(double x,double y){
        qDebug() << "MOVE: "+QString::number(x)+" : "+QString::number(y);
    }

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
    void windowDimensionsValueUpdated();
    void windowTitleValueUpdated();
    void clearColorValueChanged();

public slots:
};

#endif // COFFEERENDERER_H
